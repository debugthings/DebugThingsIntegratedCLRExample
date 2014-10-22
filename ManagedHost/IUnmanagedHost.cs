using System;
using System.Runtime.InteropServices;

namespace ADMHost
{
	/// <summary>
	///		Interface for the unmanaged half of the host to support
	/// </summary>
	[ComVisible(true),
	 Guid("fb266835-5aa4-4810-939b-dc5a0b6ab993"),
     InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IUnmanagedHost
	{
		/// <summary>
		///		Start the CLR
		/// </summary>
		void Start();

		/// <summary>
		///		Stop the CLR
		/// </summary>
		void Stop();

		/// <summary>
		///		Get the managed host of the default AppDomain
		/// </summary>
		IManagedHost DefaultManagedHost { get; }

		/// <summary>
		///		Get the managed host for a specific AppDomain
		/// </summary>
		/// <param name="appDomain">AppDomain ID</param>
        IManagedHost GetManagedHost(int appDomain);

	}
}
