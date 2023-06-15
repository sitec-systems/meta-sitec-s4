SUMMARY = "Version package"

DESCRIPTION = "Package which creates a file with all git hashes that are used"
MAINTAINER = "Robert Lehmann <robert.lehmann@sitec-systems.de>"

LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-2-Clause;md5=8bef8e6712b1be5aa76af1ebde9d6378"

PR = "r1"

S = "${WORKDIR}"

do_compile[nostamp] = "1"
do_install[nostamp] = "1"
do_package[nostamp] = "1"


def bsp_version(d):
    import time
    import json
    import subprocess
    import io

    bblayers = bb.data.getVar('BBLAYERS', d, True).split(" ")
    bblayers = list(filter(lambda x: x != '', bblayers)) # filter empty elements
    cur_dir = bb.data.getVar('THISDIR', d, True)

    hashes = []

    for layer in bblayers:
        layer_name = os.path.basename(layer)
        os.chdir(layer)
        try:
            git_hash = subprocess.check_output(['git', 'rev-parse', 'HEAD'])
        except:
            git_hash = "none"
        layer_data = {}
        layer_data['name'] = layer_name
        layer_data['hash'] = git_hash.replace("\n", "")
        hashes.append(layer_data)
    
    os.chdir(cur_dir)
    data = {}
    data['timestamp'] = time.strftime('%y%m%d_%H%M%S', time.gmtime())
    data['projects'] = hashes
    data['version' ] = time.strftime('%Y.%m', time.gmtime())

    with io.open(d.getVar('S', True) + '/bsp_version.json', 'w', encoding='utf-8') as f:
        f.write(unicode(json.dumps(data, ensure_ascii=False, indent=4)))

    
python do_compile() {
    bsp_version(d)
}

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/bsp_version.json ${D}${sysconfdir}/bsp_version.json
}
