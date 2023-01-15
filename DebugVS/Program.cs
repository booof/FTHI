using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Linq;

namespace DebugVS
{
    public class Program
    {
        static EnvDTE.DTE visual_studio_instance;

        public void echo()
        {
            Console.WriteLine(".NET Connection Active");
        }

        public void attachProcess(string solution, string symbols, int coreID)
        {
            visual_studio_instance = loadSolution(solution);

            visual_studio_instance.MainWindow.Activate();
            visual_studio_instance.MainWindow.Visible = true;
            visual_studio_instance.UserControl = true;

            EnvDTE.Processes list = visual_studio_instance.Debugger.LocalProcesses;
            foreach (EnvDTE90.Process3 proc in list)
            {
                if (proc.ProcessID == coreID)
                {
                    Console.WriteLine("Attaching To:  " + proc.ProcessID + "  " + proc.Name);
                    proc.Attach2("native");
                }
            }
        }

        public void removeProcess(string solution)
        {
            visual_studio_instance = loadSolution(solution);

            visual_studio_instance.Debugger.DetachAll();

            Console.WriteLine("Successfully Detached Debugger");
        }

        public void openVisualStudioFile(string solution, string file)
        {
            Console.WriteLine("Loading File: " + file + " In Solution: " + solution);

            visual_studio_instance = loadSolution(solution);

            visual_studio_instance.MainWindow.Activate();
            visual_studio_instance.MainWindow.Visible = true;
            visual_studio_instance.UserControl = true;

            openFile(visual_studio_instance, file);
        }

        public void openVisualStudioFileAtPoint(string solution, string file, int row, int column)
        {
            openVisualStudioFile(solution, file);

            setTextCursorPosition(row, column);
        }

        private EnvDTE.DTE loadSolution(string solution)
        {
            List<EnvDTE.DTE> visual_studio_instances = GetInstances().ToList();
            for (int i = 0; i < visual_studio_instances.Count; i++)
            {
                Console.WriteLine(visual_studio_instances[i].Solution.FullName);
                if (solution == visual_studio_instances[i].Solution.FullName)
                {
                    Console.WriteLine("Solution Currently Loaded. Opening File in Solution");
                    return visual_studio_instances[i];
                }
            }

            Console.WriteLine("Solution Not Currently Loaded. Loading Solution");

            System.Type type = Type.GetTypeFromProgID("VisualStudio.DTE");
            EnvDTE.DTE instance = (EnvDTE.DTE)System.Activator.CreateInstance(type);
            instance.Solution.Open(solution);
            return instance;
        }

        static IEnumerable<EnvDTE.DTE> GetInstances()
        {
            IRunningObjectTable rot;
            IEnumMoniker enumMoniker;
            int retVal = GetRunningObjectTable(0, out rot);

            if (retVal == 0)
            {
                rot.EnumRunning(out enumMoniker);

                uint fetched = uint.MinValue;
                IMoniker[] moniker = new IMoniker[1];
                while (enumMoniker.Next(1, moniker, (IntPtr)fetched) == 0)
                {
                    IBindCtx bindCtx;
                    CreateBindCtx(0, out bindCtx);
                    string displayName;
                    moniker[0].GetDisplayName(bindCtx, null, out displayName);
                    bool isVisualStudio = displayName.StartsWith("!VisualStudio");
                    if (isVisualStudio)
                    {
                        object obj;
                        rot.GetObject(moniker[0], out obj);
                        var dte = obj as EnvDTE.DTE;
                        yield return dte;
                    }
                }
            }
        }

        [DllImport("ole32.dll")]
        private static extern void CreateBindCtx(int reserved, out IBindCtx ppbc);

        [DllImport("ole32.dll")]
        private static extern int GetRunningObjectTable(int reserved, out IRunningObjectTable prot);

        private static void openFile(EnvDTE.DTE instance, string path)
        {
            bool complete = false;
            while (!complete)
            {
                try
                {
                    instance.ItemOperations.OpenFile(path);
                    complete = true;
                }

                catch (System.Runtime.InteropServices.COMException)
                {
                    continue;
                }
            }
        }

        private static void setTextCursorPosition(int line, int column)
        {
            bool complete = false;
            while (!complete)
            {
                try
                {
                    ((EnvDTE.TextDocument)(visual_studio_instance.ActiveDocument.Object())).Selection.MoveToLineAndOffset(line, column);
                    complete = true;
                }

                catch (System.Runtime.InteropServices.COMException)
                {
                    continue;
                }
            }
        }
    }
}

