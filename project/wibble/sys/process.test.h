/* -*- C++ -*- (c) 2007 Petr Rockai <me@mornfall.net>
               (c) 2007 Enrico Zini <enrico@enricozini.org> */

#include <wibble/sys/process.h>

#include <wibble/test.h>

using namespace std;
using namespace wibble::sys;

struct TestProcess {
    Test getcwdAndChdir() {
#ifdef POSIX
        string cwd = process::getcwd();
        process::chdir("/");
        assert_eq(process::getcwd(), string("/"));
        process::chdir(cwd);
        assert_eq(process::getcwd(), cwd);
#endif
    }

    Test umask() {
#ifdef POSIX
        mode_t old = process::umask(0012);
        assert_eq(process::umask(old), 0012u);
#endif
    }

};

// vim:set ts=4 sw=4:
