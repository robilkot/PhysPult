using PhysPult.Logic;

namespace PhysPult.Commands
{
    public class Invoker
    {
        public ICommand? Command { get; set; } = null;
        public INotifier? Notifier = null;
        public void Invoke()
        {
            try
            {
                Command?.Execute();
            }
            catch (Exception ex)
            {
                Notifier?.Notify(ex.Message);
            }
        }
    }
}
