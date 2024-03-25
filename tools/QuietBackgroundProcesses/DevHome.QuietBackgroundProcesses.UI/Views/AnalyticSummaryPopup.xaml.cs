// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.WebSockets;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

using static DevHome.QuietBackgroundProcesses.UI.ProcessData;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

public sealed partial class AnalyticSummaryPopup : ContentDialog
{
    private readonly List<ProcessData> _processDatas = new();

    public AnalyticSummaryPopup()
    {
        this.InitializeComponent();

        _processDatas.Add(new ProcessData { Process = "Powerpoint.exe", Type = ProcessType.User, CpuAboveThreshold = 1 });
        _processDatas.Add(new ProcessData { Process = "msbuild.exe", Type = ProcessType.User, CpuAboveThreshold = 2 });
        _processDatas.Add(new ProcessData { Process = "Defender", Type = ProcessType.User, CpuAboveThreshold = 3 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 4 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 5 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 6 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 7 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 8 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 9 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 10 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 11 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 12 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 13 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 14 });

        foreach (var item in _processDatas.ToList())
        {
            processListControl.ProcessDatas.Add(item);
        }
    }

    private void CloseButton_Click(object sender, RoutedEventArgs e)
    {
        this.Hide();
    }

    private void FilterTextBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        // AddRepoViewModel.FilterRepositories(FilterTextBox.Text);

        // SelectRepositories(AddRepoViewModel.EverythingToClone);

        // Filtered collection based on the condition

        // var filteredCollection = new ObservableCollection<ProcessData>(processListControl.ProcessDatas.Where(item => System.Text.RegularExpressions.Regex.IsMatch(item.Process, @"^Def.*")));

        // processListControl.ProcessDatas = new ObservableCollection<ProcessData>(processListControl.ProcessDatas.Where(item => System.Text.RegularExpressions.Regex.IsMatch(item.Process, @"^Def.*")));
        var textBox = sender as Microsoft.UI.Xaml.Controls.TextBox;

        var filterExpression = textBox.Text.Trim();

        var filteredList = new ObservableCollection<ProcessData>(_processDatas.Where(x => x.Process.Contains(filterExpression, StringComparison.OrdinalIgnoreCase)));

        processListControl.ProcessDatas.Clear();

        foreach (var item in filteredList.ToList())
        {
            processListControl.ProcessDatas.Add(item);
        }
    }
}
