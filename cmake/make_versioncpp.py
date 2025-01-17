#!/usr/bin/env python3

from __future__ import print_function

import os
import sys
from datetime import datetime
from glob import glob
from platform import system
from string import Template
from subprocess import CalledProcessError, check_output

INVALID_BUILD_NO = "???"


class Generator(object):
    def __init__(self, infile, outfile):
        self.infile = infile
        self.outfile = outfile

    def compile_output(self, template, version, branch, build_no, build_sys):
        return template.substitute(
            FreeOrion_VERSION=version,
            FreeOrion_BRANCH=branch,
            FreeOrion_BUILD_NO=build_no,
            FreeOrion_BUILDSYS=build_sys,
        )

    def execute(self, version, branch, build_no, build_sys):
        if build_no == INVALID_BUILD_NO:
            print("WARNING: Can't determine git commit!")

        if os.path.isfile(self.outfile):
            with open(self.outfile) as check_file:
                check_file_contents = check_file.read()
                if build_no == INVALID_BUILD_NO:
                    if version in check_file_contents:
                        print("Version matches version in existing Version.cpp, skip regenerating it")
                        return
                elif build_no in check_file_contents:
                    print("Build number matches build number in existing Version.cpp, skip regenerating it")
                    return

        try:
            with open(self.infile) as template_file:
                template = Template(template_file.read())
        except IOError:
            print("WARNING: Can't access %s, %s not updated!" % (self.infile, self.outfile))
            return

        print("Writing file: %s" % self.outfile)
        with open(self.outfile, "w") as generated_file:
            generated_file.write(self.compile_output(template, version, branch, build_no, build_sys))


class NsisInstScriptGenerator(Generator):
    def compile_dll_list(self):
        all_dll_files = glob("*.dll")
        accepted_dll_files = set(["GiGi.dll"])
        for dll_file in all_dll_files:
            if dll_file.startswith("boost_"):
                if dll_file.partition(".")[0] in required_boost_libraries:
                    accepted_dll_files.add(dll_file)
            else:
                accepted_dll_files.add(dll_file)
        return sorted(accepted_dll_files)

    def compile_output(self, template, version, branch, build_no, build_sys):
        dll_files = self.compile_dll_list()
        if dll_files:
            return template.substitute(
                FreeOrion_VERSION=version,
                FreeOrion_BRANCH=branch,
                FreeOrion_BUILD_NO=build_no,
                FreeOrion_BUILDSYS=build_sys,
                FreeOrion_DLL_LIST_INSTALL="\n  ".join(['File "..\\' + fname + '"' for fname in dll_files]),
                FreeOrion_DLL_LIST_UNINSTALL="\n  ".join(['Delete "$INSTDIR\\' + fname + '"' for fname in dll_files]),
                FreeOrion_PYTHON_VERSION="%d%d" % (sys.version_info.major, sys.version_info.minor),
            )

        else:
            print("WARNING: no dll files for installer package found")
            return template.substitute(
                FreeOrion_VERSION=version,
                FreeOrion_BRANCH=branch,
                FreeOrion_BUILD_NO=build_no,
                FreeOrion_BUILDSYS=build_sys,
                FreeOrion_DLL_LIST_INSTALL="",
                FreeOrion_DLL_LIST_UNINSTALL="",
                FreeOrion_PYTHON_VERSION="",
            )


if len(sys.argv) not in (3, 4):
    print("ERROR: invalid parameters.")
    print("make_versioncpp.py <project rootdir> <build system name> [<boost python suffix>]")
    quit()

os.chdir(sys.argv[1])
build_sys = sys.argv[2]
if 4 == len(sys.argv):
    boost_python_suffix = sys.argv[3]
else:
    boost_python_suffix = "%d%d" % (sys.version_info.major, sys.version_info.minor)

required_boost_libraries = [
    "boost_chrono",
    "boost_date_time",
    "boost_filesystem",
    "boost_iostreams",
    "boost_locale",
    "boost_log",
    "boost_log_setup",
    "boost_python%s" % boost_python_suffix,
    "boost_regex",
    "boost_serialization",
    "boost_signals",
    "boost_system",
    "boost_thread",
]


# A list of tuples containing generators
generators = [Generator("util/Version.cpp.in", "util/Version.cpp")]
if system() == "Windows":
    generators.append(NsisInstScriptGenerator("packaging/windows_installer.nsi.in", "packaging/windows_installer.nsi"))
if system() == "Darwin":
    generators.append(Generator("packaging/Info.plist.in", "packaging/Info.plist"))

version = "0.4.10+"
branch = ""
build_no = INVALID_BUILD_NO

try:
    branch = check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"], universal_newlines=True).strip()
    if (branch == "master") or (branch[:7] == "release"):
        branch = ""
    else:
        branch += " "
    commit = check_output(
        ["git", "show", "--no-show-signature", "-s", "--format=%h", "--abbrev=7", "HEAD"], universal_newlines=True
    ).strip()
    timestamp = float(
        check_output(
            ["git", "show", "--no-show-signature", "-s", "--format=%ct", "HEAD"], universal_newlines=True
        ).strip()
    )
    build_no = ".".join([datetime.utcfromtimestamp(timestamp).strftime("%Y-%m-%d"), commit])
except (IOError, CalledProcessError):
    print("WARNING: git not installed or not setup correctly")

for generator in generators:
    generator.execute(version, branch, build_no, build_sys)

print("Building v%s %sbuild %s" % (version, branch, build_no))
