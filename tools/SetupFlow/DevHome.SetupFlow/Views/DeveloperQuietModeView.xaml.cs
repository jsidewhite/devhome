// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using DevHome.Common;
using DevHome.Common.Extensions;
using DevHome.SetupFlow.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;

namespace DevHome.SetupFlow.Views;

public partial class DeveloperQuietModePage : ToolPage
{
    public override string ShortName => "DeveloperQuietMode";

    public DeveloperQuietModeViewModel ViewModel
    {
        get;
    }

    public DeveloperQuietModePage()
    {
        ViewModel = Application.Current.GetService<DeveloperQuietModeViewModel>();

        // InitializeComponent();
    }
}
