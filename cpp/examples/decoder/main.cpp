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

#include <openpal/logging/LogRoot.h>
#include <openpal/container/Buffer.h>

#include <opendnp3/decoder/Decoder.h>
#include <opendnp3/LogLevels.h>
#include <asiodnp3/ConsoleLogger.h>

using namespace std;
using namespace openpal;
using namespace opendnp3;
using namespace asiodnp3;

enum class Mode {
	Link,
	Transport,
	App
};

Mode GetMode(const std::string& mode)
{
	if (mode == "link")
	{
		return Mode::Link;
	}
	else if (mode == "transport")
	{
		return Mode::Transport;
	}
	else
	{
		return Mode::App;
	}
}

int main(int argc, char* argv[])
{		
	openpal::LogRoot log(&ConsoleLogger::Instance(), "decoder", LogFilters(~0));
	IDecoderCallbacks callback;
	Decoder decoder(callback, log.GetLogger());
	
	Buffer buffer(4096);

	const Mode MODE = (argc > 1) ? GetMode(argv[1]) : Mode::Link;

	while (true)
	{
		const size_t NUM_READ = fread(buffer(), 1, buffer.Size(), stdin);
		
		if (NUM_READ == 0)
		{
			return 0;
		}		
		
		switch (MODE)
		{
			case(Mode::Link):
				decoder.DecodeLPDU(buffer.ToRSlice().Take(NUM_READ));
				break;
			case(Mode::Transport) :
				decoder.DecodeTPDU(buffer.ToRSlice().Take(NUM_READ));
				break;
			default:
				decoder.DecodeAPDU(buffer.ToRSlice().Take(NUM_READ));
				break;
		}
	}

	return 0;
}

