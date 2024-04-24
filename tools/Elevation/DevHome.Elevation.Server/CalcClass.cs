// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DevHome.Elevation.Server
{
    public sealed class CalcClass
    {
        public static int Add(int x, int y)
        {
            Console.WriteLine($"Calculating {x} + {y}");
            return x + y;
        }
    }
}
