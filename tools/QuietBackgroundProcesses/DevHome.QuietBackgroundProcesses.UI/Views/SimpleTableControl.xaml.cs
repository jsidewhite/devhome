// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using Microsoft.UI;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

public sealed partial class SimpleTableControl : UserControl
{
    public ObservableCollection<ProcessData> ProcessDatas { get; } = new ObservableCollection<ProcessData>();

    public SimpleTableControl()
    {
        this.InitializeComponent();
    }
}
