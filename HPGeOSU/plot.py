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
    # Energy markers and labels
    marker1_keV = 68.754
    marker2_keV = 11.104
    marker3_keV = 10.368
    diff1_keV = abs(marker1_keV - marker2_keV)
    diff2_kev = diff1_keV - marker2_keV
    diff3_kev = diff2_kev - marker2_keV

    # Define labels for each line
    labels = {
        marker1_keV: "68.75 keV",
        marker2_keV: "Ge K edge x ray",
        marker3_keV: "Ga K edge x ray",
        diff1_keV: "68.75 keV - Ge K edge x ray",
    }

    # Full range histogram
    bins_full = int((1000 - 1e-3) / 0.02)
    hist = np.histogram(data, bins=bins_full, range=(1e-3, 1000))
    hep.histplot(hist, histtype="fill", alpha=0.5, color=color)
    hep.histplot(hist, histtype="step", color=color)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.yscale("log")
    plt.grid(True)
    plt.subplots_adjust(top=0.95, bottom=0.2)
    plt.savefig(title + ".png")
    plt.close()

    def plot_zoom(xmin, xmax, suffix):
        bins_zoom = int((xmax - xmin) / 0.02)  # 20 eV bins
        hist_zoom = np.histogram(data, bins=bins_zoom, range=(xmin, xmax))
        hep.histplot(hist_zoom, histtype="fill", alpha=0.5, color=color)
        hep.histplot(hist_zoom, histtype="step", color=color)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.yscale("log")
        plt.grid(True)

        # Define colors for each marker
        colors = {
            marker1_keV: "red",
            marker2_keV: "orange",
            marker3_keV: "green",
            diff1_keV: "purple",
        }

        # Add vertical dashed lines only if within zoom range
        y_min_zoom, y_max_zoom = plt.ylim()
        for marker, label in labels.items():
            if xmin <= marker <= xmax:
                # Get matching color for this marker
                line_color = colors[marker]

                if marker == marker3_keV:
                    x_offset = -0.4
                elif marker == marker1_keV:
                    x_offset = -0.7
                elif marker == marker2_keV:
                    x_offset = 2.1
                elif marker == diff1_keV:
                    x_offset = 1.5
                else:
                    x_offset = 1.5

                plt.axvline(marker, color=line_color, linestyle="--", linewidth=4)
                plt.text(
                    marker + x_offset,
                    y_min_zoom * 1.25,
                    label,
                    rotation=90,
                    va="bottom",
                    ha="right",
                    fontsize=40,
                    bbox=dict(
                        facecolor="white", edgecolor=line_color, boxstyle="round"
                    ),
                )
        plt.subplots_adjust(top=0.95, bottom=0.2)
        plt.savefig(title + suffix + ".png")
        plt.close()

    # Zoomed histograms
    plot_zoom(1e-3, 100, "_zoomed1")  # 1 mkeV to 100 keV
    plot_zoom(3, 17, "_zoomed2")  # 3 to 17 keV
    plot_zoom(50, 70, "_zoomed3")  # 50 to 70 keV
    plot_zoom(30, 70, "_zoomed4")  # 50 to 70 keV


def plot_histogram_no_markers(data, title, xlabel, ylabel, color="b"):
    # Energy markers and labels
    marker1_keV = 68.754
    marker2_keV = 11.104
    marker3_keV = 10.368
    diff1_keV = abs(marker1_keV - marker2_keV)
    diff2_kev = diff1_keV - marker2_keV
    diff3_kev = diff2_kev - marker2_keV

    # Define labels for each line
    labels = {
        marker1_keV: "68.75 keV",
        marker2_keV: "Ge K edge x ray",
        marker3_keV: "Ga K edge x ray",
        diff1_keV: "68.75 keV - Ge K edge x ray",
    }
    # Full range histogram
    bins_full = int((1000 - 1e-3) / 0.02)
    hist = np.histogram(data, bins=bins_full, range=(1e-3, 1000))
    hep.histplot(hist, histtype="fill", alpha=0.5, color=color)
    hep.histplot(hist, histtype="step", color=color)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.yscale("log")
    plt.grid(True)
    plt.subplots_adjust(top=0.95, bottom=0.2)
    plt.savefig(title + ".png")
    plt.close()

    def plot_zoom(xmin, xmax, suffix):
        bins_zoom = int((xmax - xmin) / 0.02)  # 20 eV bins
        hist_zoom = np.histogram(data, bins=bins_zoom, range=(xmin, xmax))
        hep.histplot(hist_zoom, histtype="fill", alpha=0.5, color=color)
        hep.histplot(hist_zoom, histtype="step", color=color)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.yscale("log")
        plt.grid(True)

        # Define colors for each marker
        colors = {
            marker1_keV: "black",
            marker2_keV: "black",
            marker3_keV: "black",
            diff1_keV: "black",
        }

        # Add vertical dashed lines only if within zoom range
        y_min_zoom, y_max_zoom = plt.ylim()
        for marker, label in labels.items():
            if xmin <= marker <= xmax:
                # Get matching color for this marker
                line_color = colors[marker]
                plt.axvline(marker, color=line_color, linestyle="--", linewidth=4)
                plt.subplots_adjust(top=0.95, bottom=0.2)
        plt.savefig(title + suffix + ".png")
        plt.close()

    # Zoomed histograms
    plot_zoom(1e-3, 100, "_zoomed1_nomarkers")  # 1 mkeV to 100 keV
    plot_zoom(3, 17, "_zoomed2_nomarkers")  # 3 to 17 keV
    plot_zoom(50, 70, "_zoomed3_nomarkers")  # 50 to 70 keV
    plot_zoom(30, 70, "_zoomed4")  # 50 to 70 keV


file_path = "5E8.root"
root_file = uproot.open(file_path)
total_edep = root_file["Energy"]["fEdep"].array(library="np")
broadened_edep = apply_energy_broadening(total_edep, hpge_resolution)
plot_histogram(
    broadened_edep,
    "Total Energy Deposition in HPGe (with broadening)",
    "Energy Deposited (keV)",
    "Counts / 20 eV",
    color="b",
)
plot_histogram_no_markers(
    broadened_edep,
    "Total Energy Deposition in HPGe (with broadening)",
    "Energy Deposited (keV)",
    "Counts / 20 eV",
    color="b",
)
