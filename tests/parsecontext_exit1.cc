// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>

void exit1(Ewoms::InputError::Action action) {
    const char * deckString =
        "RUNSPEC\n"
        "DIMENS\n"
        "  10 10 10 10 /n"
        "\n";

    Ewoms::ParseContext parseContext;
    Ewoms::Parser parser;
    Ewoms::ErrorGuard errors;

    parseContext.update(Ewoms::ParseContext::PARSE_EXTRA_DATA , action);
    parser.parseString( deckString , parseContext, errors );
}

/*
  This test checks that the application will exit with status 1 - if that is
  requested; since the boost test framework has registered atexit() handlers
  which will unconditionally fail the complete test in the face of an exit(1) -
  this test is implemented without the BOOST testing framework.
*/

void test_exit(Ewoms::InputError::Action action) {
    pid_t pid = fork();
    if (pid == 0)
        exit1(action);

    int wait_status;
    waitpid(pid, &wait_status, 0);

    if (WIFEXITED(wait_status)) {
        /*
          We *want* the child process to terminate with status exit(1), i.e. if
          the exit status is 0 we fail the complete test with exit(1).
        */
        if (WEXITSTATUS(wait_status) == 0)
            std::exit(EXIT_FAILURE);
    } else
        std::exit(EXIT_FAILURE);
}

int main() {
    test_exit(Ewoms::InputError::Action::EXIT1);
    test_exit(Ewoms::InputError::Action::DELAYED_EXIT1);
}
