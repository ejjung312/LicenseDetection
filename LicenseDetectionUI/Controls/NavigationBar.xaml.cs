using System.Windows;
using System.Windows.Controls;

namespace LicenseDetectionUI.Controls
{
    /// <summary>
    /// NavigationBar.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class NavigationBar : UserControl
    {
        public NavigationBar()
        {
            InitializeComponent();
        }

        private void Close_Click(object sender, RoutedEventArgs e)
        {
            Window window = Window.GetWindow(this);
            window.Close();
        }
    }
}
