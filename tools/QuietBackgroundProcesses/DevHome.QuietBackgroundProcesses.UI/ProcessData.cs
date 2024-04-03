// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Drawing;
using Windows.UI;

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

    public ProcessData()
    {
        Process = string.Empty;
    }

    public string Process { get; set; }

    public ProcessType Type { get; set; }

    public int CpuAboveThreshold { get; set; }
}
