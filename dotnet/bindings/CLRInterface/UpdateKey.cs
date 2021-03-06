﻿
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    public class UpdateKey
    {
        public UpdateKey(byte[] key)
        {
            this.keyMode = GetModeFromKeyLength(key.Length);
            this.key = key;            
        }

        /// <summary>
        /// Only for demo purposes
        /// </summary>
        /// <param name="repeat"></param>
        /// <param name="mode"></param>
        static public UpdateKey Demo(byte repeat, KeyWrapAlgorithm mode)
        {
            var bytes = GetBuffer(mode);
            for (int i = 0; i < bytes.Length; ++i)
            {
                bytes[i] = repeat;
            }
            return new UpdateKey(bytes);
        }

        static byte[] GetBuffer(KeyWrapAlgorithm mode)
        {
            switch (mode)
            { 
                case(KeyWrapAlgorithm.AES_128):
                    return new byte[16];
                case (KeyWrapAlgorithm.AES_256):
                    return new byte[32];
                default:
                    throw new ArgumentException(String.Format("Invalid update key mode: {0}", mode));
            }
        }

        static KeyWrapAlgorithm GetModeFromKeyLength(int length)
        {
            switch (length)
            { 
                case(16):
                    return KeyWrapAlgorithm.AES_128;
                case(32):
                    return KeyWrapAlgorithm.AES_256;
                default:
                    throw new ArgumentException(String.Format("Invalid update key length: {0}", length));
            }
        }

        public readonly KeyWrapAlgorithm keyMode;
        public readonly byte[] key;        
    };   
}

