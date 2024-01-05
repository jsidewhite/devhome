// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

namespace DevHome.Experiments.QuietBackgroundProcesses;

public class QuietBackgroundProcessesCsharpApi
{
    private static bool _quietBackgroundProcessesEnabled;
    private static int _myint;

    public static bool QuietBackgroundProcessesEnabled
    {
        get
        {
            return _quietBackgroundProcessesEnabled;
        }

        set
        {
            _myint = PInvokeApiPublic.EnableQuietBackgroundProcesses();
            _quietBackgroundProcessesEnabled = value;
        }
    }
}
