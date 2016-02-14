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
#include <asiodnp3/MeasUpdate.h>

#include <asiopal/UTCTimeSource.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>

#include <opendnp3/outstation/Database.h>

#include <opendnp3/LogLevels.h>

#include <unistd.h>

#include <string>
#include <thread>
#include <iostream>

using namespace std;
using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;

const int inst0 = 0;
const int inst1 = 1;
const int inst2 = 2;
const int insts = 3;

const unsigned num_binary_vars = 3;
const unsigned num_binary = num_binary_vars * 3;
    const int bin_packed1 = 0;
    const int bin_packed2 = 1;
    const int bin_w_flag = 2;

const unsigned num_double_binary_vars = 2;
const unsigned num_double_binary = num_double_binary_vars * 3;
    const int dbin_packed1 = 0;
    const int dbin_packed2 = 1;

const unsigned num_binary_output_vars = 2;
const unsigned num_binary_output = num_binary_output_vars * 3;
    const int bino1 = 0;
    const int bino2 = 1;

const unsigned num_analog_vars = 6;
const unsigned num_analog = num_analog_vars * 3;
    const int b32_w_flag = 0;
    const int b16_w_flag = 1;
    const int b32_wo_flag = 2;
    const int b16_wo_flag = 3;
    const int sp_w_flag = 4;
    const int dp_w_flag = 5;

const unsigned num_counter_vars = 4;
const unsigned num_counter = num_counter_vars * 3;
    const int cntr32_w_flag = 0;
    const int cntr16_w_flag = 1;
    const int cntr32_wo_flag = 2;
    const int cntr16_wo_flag = 3;

const unsigned num_frozen_counter_vars = 5;
const unsigned num_frozen_counter = num_frozen_counter_vars * 3;
    const int fcntr32_w_flag = 0;
    const int fcntr16_w_flag_time = 1;
    const int fcntr32_wo_flag = 2;

const unsigned num_analog_output_vars = 6;
const unsigned num_analog_output = num_analog_output_vars * 3;
    const int ao32_w_flag = 0;
    const int ao16_w_flag = 1;
    const int aoSP_w_flag = 4;
    const int aoDP_w_flag = 5;

const unsigned num_time_and_interval_vars = 1;
const unsigned num_time_and_interval = 3;

void ConfigureDatabase(DatabaseConfigView view)
{
    for (int i = 0; i < insts; ++i) {
        view.binaries[bin_packed1 + (i * num_binary_vars)].variation = StaticBinaryVariation::Group1Var1;
        view.binaries[bin_packed2 + (i * num_binary_vars)].variation = StaticBinaryVariation::Group1Var1;
        view.binaries[bin_w_flag + (i * num_binary_vars)].variation = StaticBinaryVariation::Group1Var2;

        view.doubleBinaries[dbin_packed1 + (i * num_double_binary_vars)].variation = StaticDoubleBinaryVariation::Group3Var2;
        view.doubleBinaries[dbin_packed2 + (i * num_double_binary_vars)].variation = StaticDoubleBinaryVariation::Group3Var2;

        view.binaryOutputStatii[bino1 + (i * num_binary_vars)].variation = StaticBinaryOutputStatusVariation::Group10Var2;
        view.binaryOutputStatii[bino1 + (i * num_binary_vars)].metadata.variation = EventBinaryOutputStatusVariation::Group11Var1; // status w/o time
        view.binaryOutputStatii[bino2 + (i * num_binary_vars)].variation = StaticBinaryOutputStatusVariation::Group10Var2;
        view.binaryOutputStatii[bino2 + (i * num_binary_vars)].metadata.variation = EventBinaryOutputStatusVariation::Group11Var2; // status w/ time

        view.analogs[b32_w_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var1;
        view.analogs[b32_w_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var1; // 32 w/o time
        view.analogs[b16_w_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var2;
        view.analogs[b16_w_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var2; // 16 w/o time
        view.analogs[b32_wo_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var3;
        view.analogs[b32_wo_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var3; // 32 w/ time
        view.analogs[b16_wo_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var4;
        view.analogs[b16_wo_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var4; // 16 w/ time
        view.analogs[sp_w_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var5;
        view.analogs[sp_w_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var5; // SP w/o time
        view.analogs[dp_w_flag + (i * num_analog_vars)].variation = StaticAnalogVariation::Group30Var6;
        view.analogs[dp_w_flag + (i * num_analog_vars)].metadata.variation = EventAnalogVariation::Group32Var8; // DP w/ time

        view.counters[cntr32_w_flag + (i * num_counter_vars)].variation = StaticCounterVariation::Group20Var1;
        view.counters[cntr32_w_flag + (i * num_counter_vars)].metadata.variation = EventCounterVariation::Group22Var1;
        view.counters[cntr16_w_flag + (i * num_counter_vars)].variation = StaticCounterVariation::Group20Var2;
        view.counters[cntr16_w_flag + (i * num_counter_vars)].metadata.variation = EventCounterVariation::Group22Var2;
        view.counters[cntr32_wo_flag + (i * num_counter_vars)].variation = StaticCounterVariation::Group20Var5;
        view.counters[cntr32_wo_flag + (i * num_counter_vars)].metadata.variation = EventCounterVariation::Group22Var5;
        view.counters[cntr16_wo_flag + (i * num_counter_vars)].variation = StaticCounterVariation::Group20Var6;
        view.counters[cntr16_wo_flag + (i * num_counter_vars)].metadata.variation = EventCounterVariation::Group22Var6;

        view.frozenCounters[fcntr32_w_flag + (i * num_frozen_counter_vars)].variation = StaticFrozenCounterVariation::Group21Var1;
        view.frozenCounters[fcntr32_w_flag + (i * num_frozen_counter_vars)].metadata.variation = EventFrozenCounterVariation::Group23Var1;
        view.frozenCounters[fcntr16_w_flag_time + (i * num_frozen_counter_vars)].variation = StaticFrozenCounterVariation::Group21Var6;
        view.frozenCounters[fcntr16_w_flag_time + (i * num_frozen_counter_vars)].metadata.variation = EventFrozenCounterVariation::Group23Var6;
        view.frozenCounters[fcntr32_wo_flag + (i * num_frozen_counter_vars)].variation = StaticFrozenCounterVariation::Group21Var9;
        view.frozenCounters[fcntr32_wo_flag + (i * num_frozen_counter_vars)].metadata.variation = EventFrozenCounterVariation::Group23Var5;

        view.analogOutputStatii[ao32_w_flag + (i * num_analog_output_vars)].variation = StaticAnalogOutputStatusVariation::Group40Var1;
        view.analogOutputStatii[ao32_w_flag + (i * num_analog_output_vars)].metadata.variation = EventAnalogOutputStatusVariation::Group42Var1;
        view.analogOutputStatii[ao16_w_flag + (i * num_analog_output_vars)].variation = StaticAnalogOutputStatusVariation::Group40Var2;
        view.analogOutputStatii[ao16_w_flag + (i * num_analog_output_vars)].metadata.variation = EventAnalogOutputStatusVariation::Group42Var4;
        view.analogOutputStatii[aoSP_w_flag + (i * num_analog_output_vars)].variation = StaticAnalogOutputStatusVariation::Group40Var3;
        view.analogOutputStatii[ao32_w_flag + (i * num_analog_output_vars)].metadata.variation = EventAnalogOutputStatusVariation::Group42Var5;
        view.analogOutputStatii[aoDP_w_flag + (i * num_analog_output_vars)].variation = StaticAnalogOutputStatusVariation::Group40Var4;
        view.analogOutputStatii[ao32_w_flag + (i * num_analog_output_vars)].metadata.variation = EventAnalogOutputStatusVariation::Group42Var8;
    }
//      example of configuring analog index 0 for Class2 with floating point variations by default
//	view.analogs[0].variation = StaticAnalogVariation::Group30Var5;
//	view.analogs[0].metadata.clazz = PointClass::Class2;
//	view.analogs[0].metadata.variation = EventAnalogVariation::Group32Var7;
}

void UpdateBinary(MeasUpdate &tx, int phase)
{
    static bool set = false;
    for (int i = 0; i < insts; ++i) {
        bool use_static = ((i == inst0) || (i == inst2 && phase != 2));
        bool bv1, bv2, bv3;
        if (use_static) {
            bv1 = false;
            bv2 = true;
            bv3 = true;
        } else {
            bv1 = !set;
            bv2 = set;
            bv3 = set;
        }
        tx.Update(Binary(bv1), bin_packed1 + (i * num_binary_vars));
        tx.Update(Binary(bv2), bin_packed2 + (i * num_binary_vars));
        tx.Update(Binary(bv3), bin_w_flag + (i * num_binary_vars));
    }
    set = !set;
}

void UpdateDoubleBinary(MeasUpdate &tx, int phase)
{
    // inst0 stays constant all the time
    // inst1 rotates among 3 states
    // inst2 stays constant in phase1, rotates in phase2
    static DoubleBit states[] = { DoubleBit::DETERMINED_OFF,
                                  DoubleBit::INTERMEDIATE,
                                  DoubleBit::DETERMINED_ON };
    static int idx = 0;
    for (int i = 0; i < insts; ++i) {
        bool use_static = ((i == inst0) || (i == inst2 && phase != 2));
        DoubleBit dbv1, dbv2;
        if (use_static) {
            dbv1 = DoubleBit::DETERMINED_OFF;
            dbv2 = DoubleBit::DETERMINED_ON;
        } else {
            dbv1 = dbv2 = states[idx];
        }
        tx.Update(DoubleBitBinary(dbv1), dbin_packed1 + (i * num_binary_vars));
        tx.Update(DoubleBitBinary(dbv2), dbin_packed2 + (i * num_binary_vars));
    }
    idx = (idx + 1) % 3;
}

void UpdateAnalog(MeasUpdate &tx, int phase)
{
    static uint16_t b16_static = 0x200;
    static uint32_t b32_static = 0x200000;
    static double f_static = 5.0;
    static uint16_t b16_rstates[] = { 0x100, 0x200, 0x300 };
    static uint32_t b32_rstates[] = { 0x100000, 0x200000, 0x300000 };
    static double f_rstates[] = { -500000.0, 0.0, 500000.0 };
    static int idx = 0;
    for (int i = 0; i < insts; ++i) {
        bool use_static = ((i == inst0) || (i == inst2 && phase != 2));
        tx.Update(Analog(use_static ? b32_static : b32_rstates[idx]), b32_w_flag + (i * num_binary_vars));
        tx.Update(Analog(use_static ? b16_static : b16_rstates[idx]), b16_w_flag + (i * num_binary_vars));
        tx.Update(Analog(use_static ? b32_static : b32_rstates[idx]), b32_wo_flag + (i * num_binary_vars));
        tx.Update(Analog(use_static ? b16_static : b16_rstates[idx]), b16_wo_flag + (i * num_binary_vars));
        tx.Update(Analog(use_static ? f_static : f_rstates[idx]), sp_w_flag + (i * num_binary_vars));
        tx.Update(Analog(use_static ? f_static : f_rstates[idx]), dp_w_flag + (i * num_binary_vars));
    }
    idx = (idx + 1) % 3;
}

void UpdateCounter(MeasUpdate &tx, int phase)
{
    // Actually varying the rate of change here. Values continue to count up
    // forever (wrapping at 2^32).
    static uint32_t base32_value = 0;
    static uint16_t base16_value = 0;
    static uint32_t accel32_value = 0;
    static uint16_t accel16_value = 0;
    static uint32_t base32_delta = 100;
    static uint16_t base16_delta = 100;
    static uint32_t accel32_delta = 200;
    static uint16_t accel16_delta = 200;
    for (int i = 0; i < insts; ++i) {
        bool use_base = ((i == inst0) || (i == inst2 && phase != 2));
        tx.Update(Counter(use_base ? base32_value : accel32_value), cntr32_w_flag + (i * num_binary_vars));
        tx.Update(Counter(use_base ? base16_value : accel16_value), cntr16_w_flag + (i * num_binary_vars));
        tx.Update(Counter(use_base ? base32_value : accel32_value), cntr32_wo_flag + (i * num_binary_vars));
        tx.Update(Counter(use_base ? base16_value : accel16_value), cntr16_wo_flag + (i * num_binary_vars));
    }

    base32_value += base32_delta;
    accel32_value += accel32_delta;
    base16_value += base16_delta;
    accel16_value += accel16_delta;
}

void UpdateTimeAndInterval(MeasUpdate &tx, int phase)
{
}

void UpdateValues(IOutstation *pOutstation, int phase)
{
    MeasUpdate tx(pOutstation);

    UpdateBinary(tx, phase);
    UpdateDoubleBinary(tx, phase);
    UpdateAnalog(tx, phase);
    UpdateCounter(tx, phase);
    UpdateTimeAndInterval(tx, phase);
}

int main(int argc, char* argv[])
{

	// Specify what log levels to use. NORMAL is warning and above
	// You can add all the comms logging by uncommenting below.
	const uint32_t FILTERS = levels::NORMAL; // | levels::ALL_COMMS;

        int phase = 1;
        if (argc > 1 && string(argv[1]) == "-a")
            phase = 2;

	// This is the main point of interaction with the stack
	// Allocate a single thread to the pool since this is a single outstation
	DNP3Manager manager(1);

	// send log messages to the console
	manager.AddLogSubscriber(ConsoleLogger::Instance());

	// Create a TCP server (listener)
	auto pChannel = manager.AddTCPServer("server", FILTERS, ChannelRetry::Default(), "0.0.0.0", 20000);

	// Optionally, you can bind listeners to the channel to get state change notifications
	// This listener just prints the changes to the console
	pChannel->AddStateListener([](ChannelState state)
	{
		std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
	});

	// The main object for a outstation. The defaults are useable,
	// but understanding the options are important.
	OutstationStackConfig stackConfig;

        const unsigned num_per_object = 10;

	// You must specify the shape of your database and the size of the event buffers
        stackConfig.dbTemplate = DatabaseTemplate(num_binary, num_double_binary, num_analog,
                                num_counter, num_frozen_counter, num_binary_output,
                                num_analog_output, num_time_and_interval);
	stackConfig.outstation.eventBufferConfig = EventBufferConfig::AllTypes(num_per_object);

	// you can override an default outstation parameters here
	// in this example, we've enabled the oustation to use unsolicted reporting
	// if the master enables it
	stackConfig.outstation.params.allowUnsolicited = true;

	// You can override the default link layer settings here
	// in this example we've changed the default link layer addressing
	stackConfig.link.LocalAddr = 10;
	stackConfig.link.RemoteAddr = 1;

	// Create a new outstation with a log level, command handler, and
	// config info this	returns a thread-safe interface used for
	// updating the outstation's database.
	auto pOutstation = pChannel->AddOutstation("outstation", SuccessCommandHandler::Instance(), DefaultOutstationApplication::Instance(), stackConfig);

	// You can optionally change the default reporting variations or class assignment prior to enabling the outstation
	ConfigureDatabase(pOutstation->GetConfigView());

	// Enable the outstation and start communications
	pOutstation->Enable();

        const unsigned update_interval = 100000;

        while (true) {
            UpdateValues(pOutstation, phase);
            usleep(update_interval);
        }

	return 0;
}
