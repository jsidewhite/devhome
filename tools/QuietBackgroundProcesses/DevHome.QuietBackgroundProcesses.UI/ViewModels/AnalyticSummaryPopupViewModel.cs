// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Globalization;
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

    public AdvancedCollectionView ProcessDatasAd { get; private set; }

    public AnalyticSummaryPopupViewModel(QuietBackgroundProcesses.ProcessPerformanceTable? performanceTable)
    {
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
            var entry2 = new ProcessData { Process = "sdf", Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 0 };
            _processDatas.Add(entry2);

            var rows = performanceTable.Rows;
            foreach (var row in rows)
            {
                if (row != null)
                {
                    // _processDatas.Add(new ProcessData { Process = row.Name, Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = 14 });
                    var entry = new ProcessData { Process = row.Name, Type = DevHome.QuietBackgroundProcesses.UI.ProcessData.ProcessType.User, CpuAboveThreshold = (int)row.CpuTimeAboveThreshold };
                    _processDatas.Add(entry);
                }
            }
        }

        ProcessDatasAd = new AdvancedCollectionView(_processDatas, true);
    }

    public void FilterProcesses(string filterExpression)
    {
        ProcessDatasAd.Filter = item =>
        {
            try
            {
                if (item is DevHome.QuietBackgroundProcesses.UI.ProcessData process)
                {
                    return
                        process.Process.Contains(filterExpression, StringComparison.OrdinalIgnoreCase)
                        || process.Type.ToString().Contains(filterExpression, StringComparison.OrdinalIgnoreCase)
                        || process.CpuAboveThreshold.ToString(CultureInfo.InvariantCulture).Contains(filterExpression, StringComparison.OrdinalIgnoreCase);
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

    public async Task<bool> PickConfigurationFileAsync()
    {
        // Get the application root window.
        var mainWindow = Application.Current.GetService<WindowEx>();

        // Create and configure file picker
        // Log.Logger?.ReportInfo(Log.Component.Configuration, "Launching file picker to select configuration file");
        // var file = await mainWindow.OpenFilePickerAsync(Log.Logger, ("*.yaml;*.yml", StringResource.GetLocalized(StringResourceKey.FilePickerFileTypeOption, "YAML")));
        // var file = await mainWindow.OpenFilePickerAsync(Log.Logger, ("*.yaml;*.yml", "YAML"));
        var file = await mainWindow.OpenFileSaveDialogAsync(null, $"analyticSummary-{DateTime.Now:yyyy-MM-dd_HH-mm}", ("*.json", "JSON"));

        // Check if a file was selected
        if (file == null)
        {
            // Log.Logger?.ReportInfo(Log.Component.Configuration, "No configuration file selected");
        }

        return false;
    }

    [RelayCommand]
    public void SaveReportButtonClicked()
    {
        var x = PickConfigurationFileAsync().Result;
    }
}
