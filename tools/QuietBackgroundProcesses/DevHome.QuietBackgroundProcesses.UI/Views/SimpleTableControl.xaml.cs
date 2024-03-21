// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using Microsoft.UI.Xaml.Controls;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

public sealed partial class SimpleTableControl : UserControl
{
    public ObservableCollection<ProcessData> ProcessDatas { get; } = new ObservableCollection<ProcessData>();

    public SimpleTableControl()
    {
        this.InitializeComponent();
    }
}
