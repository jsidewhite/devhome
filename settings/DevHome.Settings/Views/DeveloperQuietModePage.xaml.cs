// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using DevHome.Common.Extensions;
using DevHome.Common.Services;
using DevHome.Settings.Models;
using DevHome.Settings.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace DevHome.Settings.Views;

public sealed partial class DeveloperQuietModePage : Page
{
    public DeveloperQuietModeViewModel ViewModel
    {
        get;
    }

    public ObservableCollection<Breadcrumb> Breadcrumbs
    {
        get;
    }

    public DeveloperQuietModePage()
    {
        ViewModel = Application.Current.GetService<DeveloperQuietModeViewModel>();
        this.InitializeComponent();

        var stringResource = new StringResource("DevHome.Settings/Resources");
        Breadcrumbs = new ObservableCollection<Breadcrumb>
        {
            new Breadcrumb(stringResource.GetLocalized("Settings_Header"), typeof(SettingsViewModel).FullName!),
            new Breadcrumb(stringResource.GetLocalized("Settings_DeveloperQuietModePage_Header"), typeof(DeveloperQuietModeViewModel).FullName!),
        };
    }

    private void BreadcrumbBar_ItemClicked(BreadcrumbBar sender, BreadcrumbBarItemClickedEventArgs args)
    {
        if (args.Index < Breadcrumbs.Count - 1)
        {
            var crumb = (Breadcrumb)args.Item;
            crumb.NavigateTo();
        }
    }
}
