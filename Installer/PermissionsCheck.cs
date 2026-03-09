using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Text;

namespace Installer
{
    internal class PermissionsCheck
    {
        const int FILE_GENERIC_WRITE = 0x40000000;

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool AccessCheck(
            IntPtr pSecurityDescriptor,
            IntPtr ClientToken,
            int DesiredAccess,
            ref GENERIC_MAPPING GenericMapping,
            IntPtr PrivilegeSet,
            ref int PrivilegeSetLength,
            out int GrantedAccess,
            out bool AccessStatus);

        [StructLayout(LayoutKind.Sequential)]
        struct GENERIC_MAPPING
        {
            public int GenericRead;
            public int GenericWrite;
            public int GenericExecute;
            public int GenericAll;
        }

        public static bool CanWrite(string folder)
        {
            DirectorySecurity security = new DirectoryInfo(folder).GetAccessControl();
            byte[] sd = security.GetSecurityDescriptorBinaryForm();

            IntPtr pSD = Marshal.AllocHGlobal(sd.Length);
            Marshal.Copy(sd, 0, pSD, sd.Length);

            WindowsIdentity identity = WindowsIdentity.GetCurrent();
            IntPtr token = identity.Token;

            GENERIC_MAPPING mapping = new()
            {
                GenericRead = 0x120089,
                GenericWrite = 0x120116,
                GenericExecute = 0x1200A0,
                GenericAll = 0x1F01FF
            };

            int privilegeLength = 0;

            AccessCheck(
                pSD,
                token,
                FILE_GENERIC_WRITE,
                ref mapping,
                IntPtr.Zero,
                ref privilegeLength,
                out int granted,
                out bool status);

            Marshal.FreeHGlobal(pSD);

            return status;
        }
    }
}
