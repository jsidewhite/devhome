// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DevHome.Experiments.QuietBackgroundProcesses;

internal class PInvokeApi
{
    [DllImport("QuietBackgroundProcessesApi.dll")]
    private static extern int EnableQuietBackgroundProcesses();
}

public class PInvokeApiPublic
{
    public static int EnableQuietBackgroundProcesses()
    {
        return 342;
    }
}
