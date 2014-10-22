using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Security;
using System.Security.Permissions;
using System.Security.Policy;
namespace ADMHost
{
	/// <summary>
	///		Managed half of the CLR host
	/// </summary>
	public class ManagedHost : AppDomainManager, IManagedHost
	{

        public static StrongName CreateStrongName(Assembly assembly)
        {
            if (assembly == null)
                throw new ArgumentNullException("assembly");

            AssemblyName assemblyName = assembly.GetName();
            Debug.Assert(assemblyName != null, "Could not get assembly name");

            // get the public key blob
            byte[] publicKey = assemblyName.GetPublicKey();
            if (publicKey == null || publicKey.Length == 0)
                throw new InvalidOperationException("Assembly is not strongly named");

            StrongNamePublicKeyBlob keyBlob = new StrongNamePublicKeyBlob(publicKey);

            // and create the StrongName
            return new StrongName(keyBlob, assemblyName.Name, assemblyName.Version);
        }

		private IUnmanagedHost unmanagedHost = null;

		/// <summary>
		///		A new AppDomain has been created
		/// </summary>
		public override void InitializeNewDomain(AppDomainSetup appDomainInfo)
		{
			// let the unmanaged host know about us
			InitializationFlags = AppDomainManagerInitializationOptions.RegisterWithHost;

			return;
		}

		/// <summary>
		///		Create a new AppDomain
		/// </summary>
		/// <param name="name">name of the AppDomain to create</param>
		/// <returns>ID of the new AppDomain</returns>
		int IManagedHost.CreateAppDomain(string name)
		{
            PermissionSet permissions = new PermissionSet(PermissionState.None);
            permissions.AddPermission(new SecurityPermission(PermissionState.Unrestricted));
            permissions.AddPermission(new UIPermission(PermissionState.Unrestricted));

            return AppDomain.CreateDomain(
                name,
                AppDomain.CurrentDomain.Evidence,
                AppDomain.CurrentDomain.SetupInformation,
                permissions,
                CreateStrongName(Assembly.GetExecutingAssembly())).Id;

			//return AppDomain.CreateDomain(name).Id;
		}

		/// <summary>
		///		Clean up the AppDomianManager
		/// </summary>
		void IManagedHost.Dispose()
		{
			// do any clean up work here
			return;
		}

		/// <summary>
		///		Set the unmanaged host for the AppDomainManager to work with
		/// </summary>
		/// <param name="unmanagedHost">unmanaged half of the host</param>
		void IManagedHost.SetUnmanagedHost(IUnmanagedHost unmanagedHost)
		{
			Debug.Assert(unmanagedHost != null, "Attempt to set null unmanaged host");
			Debug.Assert(this.unmanagedHost == null, "Attempt to reset unmanaged host");

			this.unmanagedHost = unmanagedHost;
			return;
		}

		/// <summary>
		///		Write a message
		/// </summary>
		/// <param name="message">message to write</param>
		void IManagedHost.Write(string message)
		{
            Trace.WriteLine(message);
			Console.WriteLine(message);
			return;
		}

        void IManagedHost.Run(string path)
        {
            new FileIOPermission(PermissionState.Unrestricted).Assert();
            string fullPath = Path.Combine(
                Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                path);
            CodeAccessPermission.RevertAssert();

            new FileIOPermission(
                FileIOPermissionAccess.Read | FileIOPermissionAccess.PathDiscovery,
                fullPath).Assert();
            AppDomain.CurrentDomain.ExecuteAssembly(fullPath);
            CodeAccessPermission.RevertAssert();
        }
	}
}
