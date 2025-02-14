SUMMARY = "Toyota IVI Homescreen"
DESCRIPTION = "Toyota's Flutter Embedder that communicates with AGL-compositor/Wayland compositors"
AUTHOR = "joel.winarske@toyotaconnected.com"
HOMEPAGE = "https://github.com/toyota-connected/ivi-homescreen"
BUGTRACKER = "https://github.com/toyota-connected/ivi-homescreen/issues"
SECTION = "graphics"
CVE_PRODUCT = "homescreen"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=39ae29158ce710399736340c60147314"

DEPENDS += "\
    compiler-rt \
    libcxx \
    libxkbcommon \
    virtual/egl \
    wayland \
    wayland-native \
    wayland-protocols \
    "

REQUIRED_DISTRO_FEATURES = "wayland opengl"

SRCREV ??= "${AUTOREV}"
SRC_URI = "git://github.com/toyota-connected/ivi-homescreen.git;protocol=https;branch=main \
           file://homescreen.service.in \
           file://custom_devices.json \
          "

S = "${WORKDIR}/git"

inherit cmake features_check systemd

RUNTIME = "llvm"
TOOLCHAIN = "clang"
PREFERRED_PROVIDER_libgcc = "compiler-rt"

EXTRA_OECMAKE += "-D CMAKE_SYSROOT=${STAGING_DIR_TARGET}/usr"

FLUTTER_RUNTIME ??= "release"
SERVICE_EXEC_START ??= "/usr/bin/homescreen --f"
PACKAGECONFIG ??= "${@bb.utils.filter('DISTRO_FEATURES', 'systemd', d)}"

do_install:append() {
    if ${@bb.utils.contains('PACKAGECONFIG', 'systemd', 'true', 'false', d)}; then
        install -m 644 ${WORKDIR}/homescreen.service.in ${WORKDIR}/homescreen.service
        sed -i "s|@ExecStart@|${SERVICE_EXEC_START}|g" ${WORKDIR}/homescreen.service
        install -d ${D}${systemd_system_unitdir}
        install -m 644 ${WORKDIR}/homescreen.service ${D}${systemd_system_unitdir}/homescreen.service
    fi
    if ${@bb.utils.contains('FLUTTER_RUNTIME', 'debug', 'true', 'false', d)}; then
        install -d ${D}${datadir}/homescreen
        install -m 644 ${WORKDIR}/custom_devices.json ${D}${datadir}/homescreen
    fi
}

SYSTEMD_SERVICE:${PN} += "${@bb.utils.contains('PACKAGECONFIG', 'systemd', 'homescreen.service', '', d)}"
SYSTEMD_PACKAGES = "${@bb.utils.contains('PACKAGECONFIG', 'systemd', '${PN}', '', d)}"
