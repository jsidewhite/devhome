// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

namespace DevHome.Experiments.QuietBackgroundProcesses;

public class QuietBackgroundProcessesCsharpApi
{
    private static bool _quietBackgroundProcessesEnabled;

    public static bool QuietBackgroundProcessesEnabled
    {
        get => _quietBackgroundProcessesEnabled;
        set => _quietBackgroundProcessesEnabled = value;
    }
}
