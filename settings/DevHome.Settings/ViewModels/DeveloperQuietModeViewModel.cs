// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using CommunityToolkit.Mvvm.ComponentModel;
using DevHome.Common.Extensions;
using DevHome.Common.Services;
using Microsoft.UI.Xaml;

namespace DevHome.Settings.ViewModels;

public partial class DeveloperQuietModeViewModel : ObservableObject
{
    [ObservableProperty]
    private string _versionDescription;

    public DeveloperQuietModeViewModel()
    {
        _versionDescription = GetVersionDescription();
    }

    private static string GetVersionDescription()
    {
        var appInfoService = Application.Current.GetService<IAppInfoService>();
        var version = appInfoService.GetAppVersion();

        return $"{version.Major}.{version.Minor}.{version.Build}.{version.Revision}";
    }
}
