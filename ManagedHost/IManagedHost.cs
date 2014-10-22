using System;
using System.Runtime.InteropServices;

namespace ADMHost
{
	/// <summary>
	///		Interface for the managed half of the CLR host to support
	/// </summary>
	[ComVisible(true),
	 Guid("5598179a-7f8f-465d-be12-da9c1f0aa115"),
     InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IManagedHost
	{
		/// <summary>
		///		Create a new AppDomain
		/// </summary>
		/// <param name="name">name of the AppDomain to create</param>
		/// <returns>ID of the new AppDomain</returns>
		int CreateAppDomain([MarshalAs(UnmanagedType.BStr)]string name);

		/// <summary>
		///		Clean up the AppDomianManager
		/// </summary>
		void Dispose();

		/// <summary>
		///		Set the unmanaged host for the AppDomainManager to work with
		/// </summary>
		/// <param name="unmanagedHost">unmanaged half of the host</param>
		void SetUnmanagedHost(IUnmanagedHost unmanagedHost);

		/// <summary>
		///		Write a message
		/// </summary>
		/// <param name="message">message to write</param>
		void Write([MarshalAs(UnmanagedType.BStr)]string message);


        /// <summary>
        ///		Write a message
        /// </summary>
        /// <param name="message">message to write</param>
        void Run([MarshalAs(UnmanagedType.BStr)]string message);
	}
}
