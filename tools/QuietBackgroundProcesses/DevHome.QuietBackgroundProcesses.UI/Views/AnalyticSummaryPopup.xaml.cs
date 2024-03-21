// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

using static DevHome.QuietBackgroundProcesses.UI.ProcessData;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

public sealed partial class AnalyticSummaryPopup : ContentDialog
{
    public AnalyticSummaryPopup()
    {
        this.InitializeComponent();

        processListControl.ProcessDatas.Add(new ProcessData { Process = "Powerpoint.exe", Type = ProcessType.User, CpuAboveThreshold = 10 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msbuild.exe", Type = ProcessType.Developer, CpuAboveThreshold = 30 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "Defender", Type = ProcessType.System, CpuAboveThreshold = 60 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
        processListControl.ProcessDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 25 });
    }

    private void CloseButton_Click(object sender, RoutedEventArgs e)
    {
        this.Hide();
    }
}
