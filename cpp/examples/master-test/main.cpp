/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/DefaultMasterApplication.h>
#include <asiodnp3/PrintingCommandCallback.h>

#include <asiopal/UTCTimeSource.h>

#include <opendnp3/LogLevels.h>

#include <unistd.h>

using namespace std;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;
using namespace opendnp3;

static char *pgm;

static void usage(void)
{
    std::cout << "Usage: " << pgm << " outstation [-a]" << std::endl;
}

int main(int argc, char* argv[])
{
    pgm = argv[0];

    if (argc < 2)
        usage();

    const string host{argv[1]};
    const int phase2 = ((argc > 2) && (string{argv[2]} == "-a"));

    // Specify what log levels to use. NORMAL is warning and above
    // You can add all the comms logging by uncommenting below
    const uint32_t FILTERS = levels::NORMAL | levels::ALL_APP_COMMS;

    // This is the main point of interaction with the stack
    DNP3Manager manager(1);

    // send log messages to the console
    manager.AddLogSubscriber(ConsoleLogger::Instance());

    // Connect via a TCPClient socket to a outstation
    auto pChannel = manager.AddTCPClient("tcpclient", FILTERS, ChannelRetry::Default(), host, "0.0.0.0", 20000);

    // Optionally, you can bind listeners to the channel to get state change notifications
    // This listener just prints the changes to the console
    pChannel->AddStateListener([](ChannelState state)
    {
        std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
    });

    // The master config object for a master. The default are
    // useable, but understanding the options are important.
    MasterStackConfig stackConfig;

    // you can override application layer settings for the master here
    // in this example, we've change the application layer timeout to 2 seconds
    stackConfig.master.responseTimeout = TimeDuration::Seconds(2);
    stackConfig.master.disableUnsolOnStartup = true;

    // You can override the default link layer settings here
    // in this example we've changed the default link layer addressing
    stackConfig.link.LocalAddr = 1;
    stackConfig.link.RemoteAddr = 10;

    // Create a new master on a previously declared port, with a
    // name, log level, command acceptor, and config info. This
    // returns a thread-safe interface used for sending commands.
    auto pMaster = pChannel->AddMaster(
                   "master",										// id for logging
                   PrintingSOEHandler::Instance(),					// callback for data processing
                   asiodnp3::DefaultMasterApplication::Instance(),	// master application instance
                   stackConfig										// stack configuration
               );


    // do Class 0 scan once per interval
    auto integrityScan = pMaster->AddClassScan(ClassField::CLASS_0, TimeDuration::Seconds(1));

    // Enable the master. This will start communications.
    pMaster->Enable();

#if 0
class AnalogOutputInt16 : public AnalogOutput<int16_t>
class AnalogOutputInt32 : public AnalogOutput<int32_t>
class AnalogOutputFloat32 : public AnalogOutput<float>
class AnalogOutputDouble64 : public AnalogOutput<double>
#endif

    int tick = 0;
    while (true) {
        integrityScan.Demand();
        if (phase2) {
            CommandSet cmds1{
                WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON, 4, 100, 100), 0),
                WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON, 4, 100, 100), 1),
                WithIndex(ControlRelayOutputBlock(ControlCode::LATCH_ON, 40, 50, 50), 2),
                };
            pMaster->SelectAndOperate(std::move(cmds1), PrintingCommandCallback::Get());

            CommandSet cmds2{
                WithIndex(AnalogOutputInt16(0x10), 0),
                WithIndex(AnalogOutputInt16(0x100), 1),
                WithIndex(AnalogOutputInt16(0xffff), 2),
            };
            pMaster->SelectAndOperate(std::move(cmds2), PrintingCommandCallback::Get());

            CommandSet cmds3{
                WithIndex(AnalogOutputInt32(0x10000), 3),
                WithIndex(AnalogOutputInt32(0x100000), 4),
                WithIndex(AnalogOutputInt32(0xffffff), 5),
            };
            pMaster->SelectAndOperate(std::move(cmds3), PrintingCommandCallback::Get());

            CommandSet cmds4{
                WithIndex(AnalogOutputFloat32(-500.0), 6),
                WithIndex(AnalogOutputFloat32(0.0), 7),
                WithIndex(AnalogOutputFloat32(50000.0), 8),
            };
            pMaster->SelectAndOperate(std::move(cmds4), PrintingCommandCallback::Get());

            CommandSet cmds5{
                WithIndex(AnalogOutputDouble64(-50000.0), 9),
                WithIndex(AnalogOutputDouble64(1.0), 10),
                WithIndex(AnalogOutputDouble64(500000000.0), 11),
            };
            pMaster->SelectAndOperate(std::move(cmds5), PrintingCommandCallback::Get());
        } else if ((tick % 10) == 0) {
            CommandSet cmds1{
                WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON, 4, 100, 100), 0),
                WithIndex(ControlRelayOutputBlock((tick & 1) ? ControlCode::PULSE_ON : ControlCode::PULSE_OFF, 4, 100, 100), 1),
                WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON, 4, 100, 100), 2),
            };
            pMaster->SelectAndOperate(std::move(cmds1), PrintingCommandCallback::Get());

            CommandSet cmds2{
                WithIndex(AnalogOutputInt16(0x10), 0),
                WithIndex(AnalogOutputInt16(0x100), 1),
                WithIndex(AnalogOutputInt16(0x1000), 2),
            };
            pMaster->SelectAndOperate(std::move(cmds2), PrintingCommandCallback::Get());

            CommandSet cmds3{
                WithIndex(AnalogOutputInt32(0x10000), 3),
                WithIndex(AnalogOutputInt32(0x100000), 4),
                WithIndex(AnalogOutputInt32(0x1000000), 5),
            };
            pMaster->SelectAndOperate(std::move(cmds3), PrintingCommandCallback::Get());

            CommandSet cmds4{
                WithIndex(AnalogOutputFloat32(-500.0), 6),
                WithIndex(AnalogOutputFloat32(0.0), 7),
                WithIndex(AnalogOutputFloat32(500.0), 8),
            };
            pMaster->SelectAndOperate(std::move(cmds4), PrintingCommandCallback::Get());

            CommandSet cmds5{
                WithIndex(AnalogOutputDouble64(-50000.0), 9),
                WithIndex(AnalogOutputDouble64(1.0), 10),
                WithIndex(AnalogOutputDouble64(500000.0), 11),
            };
            pMaster->SelectAndOperate(std::move(cmds5), PrintingCommandCallback::Get());

        }
        usleep(1000000);
    }

    do
    {
        std::cout << "Enter a command" << std::endl;
        std::cout << "x - exits program" << std::endl;
        std::cout << "a - performs and ad-hoc range scan" << std::endl;
        std::cout << "i - integrity demand scan" << std::endl;
        std::cout << "e - exception demand scan" << std::endl;
        std::cout << "d - diable unsolcited" << std::endl;
        std::cout << "r - cold restart" << std::endl;
        std::cout << "c - send crob" << std::endl;

        char cmd;
        std::cin >> cmd;
        switch(cmd)
        {
        case('a') :
            pMaster->ScanRange(GroupVariationID(1, 2), 0, 3);
            break;
        case('d') :
            pMaster->PerformFunction("disable unsol", FunctionCode::DISABLE_UNSOLICITED,
            { Header::AllObjects(60, 2), Header::AllObjects(60, 3), Header::AllObjects(60, 4) }
                                    );
            break;
        case('r') :
        {
            auto print = [](const RestartOperationResult& result)
            {
                if(result.summary == TaskCompletion::SUCCESS) {
                    std::cout << "Success, Time: " << result.restartTime.GetMilliseconds() << std::endl;
                } else {
                    std::cout << "Failure: " << TaskCompletionToString(result.summary) << std::endl;
                }    
            };
            pMaster->Restart(RestartType::COLD, print);
            break;
        }
        case('x'):
            // C++ destructor on DNP3Manager cleans everything up for you
            return 0;
        case('i'):
            integrityScan.Demand();
            break;
        case('c'):
            {
                break;
            }
        default:
            std::cout << "Unknown action: " << cmd << std::endl;
            break;
        }
    }
    while(true);

    return 0;
}

