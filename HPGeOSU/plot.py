import uproot
import matplotlib.pyplot as plt
import numpy as np
import mplhep as hep
import matplotlib as mpl
from scipy.optimize import curve_fit
from scipy.integrate import quad

mpl.use("Agg")
hep.style.use(hep.style.ROOT)
plt.rcParams["figure.figsize"] = [16, 10]
plt.rcParams["legend.facecolor"] = "white"
plt.rcParams["legend.framealpha"] = 1
plt.rcParams["legend.edgecolor"] = "black"
plt.rcParams["legend.fancybox"] = True
plt.rcParams["legend.frameon"] = True
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = ["Liberation Serif"]

plt.rcParams.update(
    {
        "axes.titlesize": 70,  # plot title font (default ~16–18)
        "axes.labelsize": 60,  # x/y label font (default ~12–14)
        "xtick.labelsize": 42,  # x tick font (default ~10)
        "ytick.labelsize": 42,  # y tick font (default ~10)
        "legend.fontsize": 60,  # legend font (default ~10–12)
    }
)


def apply_energy_broadening(energies, resolution_function):
    mask = energies > 0
    result = np.copy(energies)

    if np.any(mask):
        valid_energies = energies[mask]
        relative_resolution = resolution_function(valid_energies)
        fwhm = relative_resolution * valid_energies
        sigma = fwhm / (2 * np.sqrt(2 * np.log(2)))

        result[mask] = np.random.normal(valid_energies, sigma)

    return result


def hpge_resolution(energy_kev):
    """HPGe 2 cm³ crystal resolution function
    Calibrated to typical small HPGe detector performance:
    - ~0.74% at 122 keV
    - ~0.28% at 661 keV
    - ~0.17% at 1332 keV
    - ~0.13% at 2615 keV
    """
    # Parameters fitted from literature data for 2 cm³ HPGe
    a = 0.000254  # constant term (noise contribution)
    b = 0.000914  # statistical term (charge collection)
    c = 0.000212  # quadratic term (incomplete charge collection)

    energy_mev = energy_kev / 1000.0

    # Convert to FWHM
    fwhm_factor = 2.355  # 2 * sqrt(2 * ln(2))
    sigma = np.sqrt(a**2 + (b**2) / energy_mev + (c**2) / (energy_mev**2))
    return fwhm_factor * sigma  # Return FWHM/E (relative resolution)


def plot_histogram(data, title, xlabel, ylabel, color="b"):
    # Energy markers in eV and their conversion to keV
    marker1_keV = 68.754
    marker2_keV = 11.104
    marker3_keV = 10.368
    diff_keV = abs(marker2_keV - marker1_keV)

    # Full range histogram
    bins_full = int((1000 - 1e-3) / 0.02)
    hist = np.histogram(data, bins=bins_full, range=(1e-3, 1000))
    hep.histplot(hist, histtype="fill", alpha=0.5, color=color)
    hep.histplot(hist, histtype="step", color=color)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.yscale("log")
    plt.grid(True)

    # Add vertical dashed lines
    plt.axvline(marker1_keV, color="k", linestyle="--", alpha=0.7)
    plt.axvline(marker2_keV, color="k", linestyle="--", alpha=0.7)
    plt.axvline(marker3_keV, color="k", linestyle="--", alpha=0.7)
    plt.axvline(diff_keV, color="k", linestyle="--", alpha=0.7)

    plt.subplots_adjust(top=0.95, bottom=0.2)
    plt.savefig(title + ".png")
    plt.close()

    # Helper function for zoomed histograms with 50 eV (0.05 keV) binning
    def plot_zoom(xmin, xmax, suffix):
        bins_zoom = int((xmax - xmin) / 0.02)  # 50 eV bins
        hist_zoom = np.histogram(data, bins=bins_zoom, range=(xmin, xmax))
        hep.histplot(hist_zoom, histtype="fill", alpha=0.5, color=color)
        hep.histplot(hist_zoom, histtype="step", color=color)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.yscale("log")
        plt.grid(True)

        # Add vertical dashed lines only if they fall within the zoom range
        if xmin <= marker1_keV <= xmax:
            plt.axvline(marker1_keV, color="k", linestyle="--", alpha=0.7)
        if xmin <= marker2_keV <= xmax:
            plt.axvline(marker2_keV, color="k", linestyle="--", alpha=0.7)
        if xmin <= marker3_keV <= xmax:
            plt.axvline(marker3_keV, color="k", linestyle="--", alpha=0.7)
        if xmin <= diff_keV <= xmax:
            plt.axvline(diff_keV, color="k", linestyle="--", alpha=0.7)

        plt.subplots_adjust(top=0.95, bottom=0.2)
        plt.savefig(title + suffix + ".png")
        plt.close()

    # Zoomed histograms with consistent 50 eV binning
    plot_zoom(1e-3, 100, "_zoomed1")  # 1 mkeV to 100 keV
    plot_zoom(3, 17, "_zoomed2")  # 3 to 17 keV
    plot_zoom(50, 70, "_zoomed3")  # 50 to 70 keV


file_path = "5E8.root"
root_file = uproot.open(file_path)
total_edep = root_file["Energy"]["fEdep"].array(library="np")

# Plot for LaBr3 Total Edep
plot_histogram(
    total_edep,
    "Total Energy Deposition in HPGe",
    "Energy Deposited (keV)",
    "Counts / 20 eV",
    color="r",
)

# Apply Gaussian broadening for LaBr3
broadened_edep = apply_energy_broadening(total_edep, hpge_resolution)
plot_histogram(
    broadened_edep,
    "Total Energy Deposition in HPGe (with broadening)",
    "Energy Deposited (keV)",
    "Counts / 20 eV",
    color="b",
)
