// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.WinUI.Animations;
using CommunityToolkit.WinUI.Collections;
using DevHome.Common.Extensions;
using DevHome.Common.Helpers;
using DevHome.Common.Services;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.DevHome.SDK;
using Windows.Storage;
using Windows.Storage.Pickers;
using WinUIEx;

using static DevHome.QuietBackgroundProcesses.UI.ProcessData;

namespace DevHome.QuietBackgroundProcesses.UI.ViewModels;

public partial class AnalyticSummaryPopupViewModel : ObservableObject
{
    private readonly List<ProcessData> _processDatas = new();
    private readonly List<DevHome.QuietBackgroundProcesses.ProcessRow> _processDatas2 = new();

    public int SelectedComputeSystemSortComboBoxIndex { get; set; }

    [ObservableProperty]
    private ObservableCollection<string> _processRowSortOptions22 = new();

    public AdvancedCollectionView ProcessDatasAd { get; private set; }

    private DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType ConvertProcessType(DevHome.QuietBackgroundProcesses.ProcessType inputType)
    {
        return (DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType)inputType;
    }

    public AnalyticSummaryPopupViewModel(QuietBackgroundProcesses.ProcessPerformanceTable? performanceTable)
    {
        ProcessRowSortOptions22 = new ObservableCollection<string>
        {
            "Process Name",
            "Process Type",
            "CPU above threshold",
        };

        /*
        _processDatas.Add(new ProcessData { Name = "Powerpoint.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 1 });
        _processDatas.Add(new ProcessData { Name = "msbuild.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.Developer, CpuAboveThreshold = 2 });
        _processDatas.Add(new ProcessData { Name = "Defender", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.System, CpuAboveThreshold = 3 });
        _processDatas.Add(new ProcessData { Name = "msteams.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 4 });
        _processDatas.Add(new ProcessData { Name = "excel.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 5 });
        _processDatas.Add(new ProcessData { Name = "msword.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 6 });
        _processDatas.Add(new ProcessData { Name = "System", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.System, CpuAboveThreshold = 7 });
        _processDatas.Add(new ProcessData { Name = "SearchIndexer.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.Background, CpuAboveThreshold = 8 });
        _processDatas.Add(new ProcessData { Name = "Code.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.Developer, CpuAboveThreshold = 209 });
        _processDatas.Add(new ProcessData { Name = "msedge.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 10 });
        _processDatas.Add(new ProcessData { Name = "msedge.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 11 });
        _processDatas.Add(new ProcessData { Name = "msedge.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 12 });
        _processDatas.Add(new ProcessData { Name = "msedge.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 13 });
        _processDatas.Add(new ProcessData { Name = "msedge.exe", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 14 });
        */

        // DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User

        /*
        _processDatas.Add(new ProcessData { Process = "Powerpoint.exe", Type = ProcessType.User, CpuAboveThreshold = 1 });
        _processDatas.Add(new ProcessData { Process = "msbuild.exe", Type = ProcessType.Developer, CpuAboveThreshold = 2 });
        _processDatas.Add(new ProcessData { Process = "Defender", Type = ProcessType.System, CpuAboveThreshold = 3 });
        _processDatas.Add(new ProcessData { Process = "msteams.exe", Type = ProcessType.User, CpuAboveThreshold = 4 });
        _processDatas.Add(new ProcessData { Process = "excel.exe", Type = ProcessType.User, CpuAboveThreshold = 5 });
        _processDatas.Add(new ProcessData { Process = "msword.exe", Type = ProcessType.User, CpuAboveThreshold = 6 });
        _processDatas.Add(new ProcessData { Process = "System", Type = ProcessType.System, CpuAboveThreshold = 7 });
        _processDatas.Add(new ProcessData { Process = "SearchIndexer.exe", Type = ProcessType.Background, CpuAboveThreshold = 8 });
        _processDatas.Add(new ProcessData { Process = "Code.exe", Type = ProcessType.Developer, CpuAboveThreshold = 209 });
        _processDatas.Add(new ProcessData { Process = "msedge.exe", Type = ProcessType.User, CpuAboveThreshold = 10 });
        _processDatas.Add(new ProcessData { Process = "msedge.exe", Type = ProcessType.User, CpuAboveThreshold = 11 });
        _processDatas.Add(new ProcessData { Process = "msedge.exe", Type = ProcessType.User, CpuAboveThreshold = 12 });
        _processDatas.Add(new ProcessData { Process = "msedge.exe", Type = ProcessType.User, CpuAboveThreshold = 13 });
        _processDatas.Add(new ProcessData { Process = "msedge.exe", Type = ProcessType.User, CpuAboveThreshold = 14 });
        */

        /*
        var engine = new QuietBackgroundProcesses.PerformanceRecorderEngine();
        var table = engine.GetProcessPerformanceTable();
        var rows = table.Rows;
        foreach (var row in rows)
        {
            _processDatas.Add(new ProcessData { Process = row.Name, Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 14 });
        }
        */
        if (performanceTable != null)
        {
            var rows = performanceTable.Rows;
            foreach (var row in rows)
            {
                if (row != null)
                {
                    // _processDatas.Add(new ProcessData { Process = row.Name, Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 14 });
                    var sampleCount = row.SampleCount;

                    // todo:jw mult SamplesAboveThreshold by sampling duration
                    var sampleDuration = 1;

                    var entry = new ProcessData
                    {
                        Pid = row.Pid,
                        Name = row.Name,
                        PackageFullName = row.PackageFullName,
                        Aumid = row.Aumid,
                        Path = row.Path,
                        Type = ConvertProcessType(row.Type),
                        CreateTime = row.CreateTime,
                        ExitTime = row.ExitTime,
                        Samples = row.SampleCount,
                        Percent = row.PercentCumulative / sampleCount,
                        StandardDeviation = (float)Math.Sqrt(row.VarianceCumulative / sampleCount),
                        Sigma4Deviation = (float)Math.Sqrt(Math.Sqrt(row.Sigma4Cumulative / sampleCount)),
                        MaxPercent = row.MaxPercent,
                        TimeAboveThreshold = TimeSpan.FromSeconds(row.SamplesAboveThreshold * sampleDuration),
                        TotalCpuTimeInMicroseconds = row.TotalCpuTimeInMicroseconds,
                    };

                    entry.TimeAboveThresholdInMinutes = entry.TimeAboveThreshold.TotalMinutes;
                    _processDatas.Add(entry);
                }
            }
        }

        ProcessDatasAd = new AdvancedCollectionView(_processDatas, true);
        ProcessDatasAd.SortDescriptions.Add(new SortDescription("Pid", SortDirection.Descending));
    }

    public void FilterProcessesTextInputChanged(string filterExpression)
    {
        ProcessDatasAd.Filter = item =>
        {
            try
            {
                if (item is DevHome.QuietBackgroundProcesses.UI.ProcessData process)
                {
                    return
                        process.Name.Contains(filterExpression, StringComparison.OrdinalIgnoreCase)
                        || process.Type.ToString().Contains(filterExpression, StringComparison.OrdinalIgnoreCase)
                        || process.TimeAboveThreshold.Minutes.ToString(CultureInfo.InvariantCulture).Contains(filterExpression, StringComparison.OrdinalIgnoreCase);
                }

                return false;
            }
            catch (Exception /*ex*/)
            {
                // Log.Logger.ReportError(Log.Component.DevDrivesListViewModel, $"Failed to filter Compute system cards. Error: {ex.Message}");
            }

            return true;
        };

        ProcessDatasAd.RefreshFilter();
    }

    [RelayCommand]
    public void SortProcessesComboBoxChanged(string selectedValue)
    {
        ProcessDatasAd.SortDescriptions.Clear();
        if (selectedValue == "Process")
        {
            ProcessDatasAd.SortDescriptions.Add(new SortDescription("Name", SortDirection.Ascending));
        }
        else if (selectedValue == "Type")
        {
            ProcessDatasAd.SortDescriptions.Add(new SortDescription("Type", SortDirection.Ascending));
        }
        else if (selectedValue == "CPU above threshold")
        {
            ProcessDatasAd.SortDescriptions.Add(new SortDescription("TimeAboveThreshold", SortDirection.Descending));
        }
    }

    public void PickConfigurationFileAsync()
    {
        // Get the application root window.
        var mainWindow = Application.Current.GetService<WindowEx>();

        // Show the file save dialog
        var file = mainWindow.OpenFileSaveDialogAsync(null, $"analyticSummary-{DateTime.Now:yyyy-MM-dd_HH-mm}.csv", ("*.csv", "CSV"));

        // Check if a file was selected
        if (file == null)
        {
            return;
        }

        // Save the report to a .csv
        using (StreamWriter writer = new StreamWriter(file))
        {
            // Write the .csv header
            writer.WriteLine("Pid,Name,Samples,Percent,StandardDeviation,Sigma4Deviation,MaxPercent,TimeAboveThreshold,TotalCpuTimeInMicroseconds,PackageFullName,Aumid,Path,Type,CreateTime,ExitTime");

            // Write each item from the list to the file
            foreach (var data in this._processDatas)
            {
                string row = $"{data.Pid},{data.Name},{data.Samples},{data.Percent},{data.StandardDeviation},{data.Sigma4Deviation},{data.MaxPercent},{data.TimeAboveThreshold},{data.TotalCpuTimeInMicroseconds},{data.PackageFullName},{data.Aumid},{data.Path},{data.Type},{data.CreateTime},{data.ExitTime}";
                writer.WriteLine(row);
            }
        }
    }

    [RelayCommand]
    public void SaveReportButtonClicked()
    {
        PickConfigurationFileAsync();
    }
}
