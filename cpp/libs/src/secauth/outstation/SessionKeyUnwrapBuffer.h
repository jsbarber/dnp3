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
#ifndef SECAUTH_SESSION_KEYUNWRAP_BUFFER_H
#define SECAUTH_SESSION_KEYUNWRAP_BUFFER_H

#include <openpal/logging/Logger.h>
#include <openpal/crypto/IKeyWrapAlgo.h>
#include <openpal/container/RSlice.h>
#include <openpal/crypto/SecureStaticBuffer.h>
#include <openpal/serialization/Serialization.h>

#include <opendnp3/objects/Group120.h>

#include "secauth/AuthSizes.h"
#include "secauth/SessionKeysView.h"



namespace secauth
{
class SessionKeyUnwrapBuffer
{
public:

	class Result
	{
	public:

		static Result Failure()
		{
			return Result();
		}

		Result(const SessionKeysView& keys, const openpal::RSlice& keyStatusObject);

		bool success;
		SessionKeysView keys;
		openpal::RSlice keyStatusObject;

	private:
		Result();
	};

	Result Unwrap(
	    openpal::IKeyWrapAlgo& algo,
	    openpal::RSlice updateKey,
	    openpal::RSlice inputData,
	    openpal::Logger* pLogger);

private:

	openpal::SecureStaticBuffer<AuthSizes::MAX_SESSION_KEY_WRAP_BUFFER_SIZE> buffer;
};

}

#endif

