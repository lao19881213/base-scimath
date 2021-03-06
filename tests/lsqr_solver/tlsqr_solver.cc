///
/// This file is part of the ASKAP software distribution.
///
/// The ASKAP software distribution is free software: you can redistribute it
/// and/or modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 2 of the License,
/// or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
///

// ASKAPsoft includes
#include <askap/askap/AskapTestRunner.h>

// Test includes
#include <LSQRSolverTest.h>
#include <MathUtilsTest.h>
#include <ModelDampingTest.h>
#include <SparseMatrixTest.h>

#include <askap/AskapLogging.h>
#include <askapparallel/AskapParallel.h>

int main(int argc, char *argv[])
{
    ASKAPLOG_INIT("");

    askap::askapparallel::AskapParallel comms(argc, const_cast<const char**>(argv));

    askapdev::testutils::AskapTestRunner runner(argv[0]);

    runner.addTest(askap::lsqr::LSQRSolverTest::suite());
    runner.addTest(askap::lsqr::MathUtilsTest::suite());
    runner.addTest(askap::lsqr::ModelDampingTest::suite());
    runner.addTest(askap::lsqr::SparseMatrixTest::suite());

    bool wasSucessful = runner.run();

    return wasSucessful ? 0 : 1;
}

