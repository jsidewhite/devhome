// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace DevHome.QuietBackgroundProcesses.UI;

public class ProcessData
{
    public enum ProcessType
    {
        User,
        System,
        Developer,
        Background,
    }

    public string Process { get; set; }

    public ProcessType Type { get; set; }

    public int CpuAboveThreshold { get; set; }
}
