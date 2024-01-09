// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using DevHome.Experiments.QuietBackgroundProcesses;

namespace ClassLibrary1;

public class Class2
{
    private readonly DevHome.Experiments.QuietBackgroundProcesses.Oven _myX;

    public Class2()
    {
        DevHome.Experiments.QuietBackgroundProcesses.Oven x = new DevHome.Experiments.QuietBackgroundProcesses.Oven();
        _myX = x;
    }

    public DevHome.Experiments.QuietBackgroundProcesses.Oven GetOven()
    {
        return _myX;
    }
}
