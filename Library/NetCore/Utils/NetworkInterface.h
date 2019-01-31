#pragma once

namespace NetCore
{
	typedef enum tagAddressType
	{
		IP_ADDRESS,
		SUBNET_MASK,
		BROADCAST_ADDRESS
	}AddressType;

	typedef enum tagNI_Type
	{
		NI_TYPE_ETHERNET_CSMACD,
		NI_TYPE_ISO88025_TOKENRING,
		NI_TYPE_FRAMERELAY,
		NI_TYPE_PPP,
		NI_TYPE_SOFTWARE_LOOPBACK,
		NI_TYPE_ATM,
		NI_TYPE_IEEE80211,
		NI_TYPE_TUNNEL,
		NI_TYPE_IEEE1394,
		NI_TYPE_OTHER
	}NI_Type;

	typedef enum tagIPVersion
	{
		IPv4_ONLY,    /// Return interfaces with IPv4 address only
		IPv6_ONLY,    /// Return interfaces with IPv6 address only
		IPv4_OR_IPv6  /// Return interfaces with IPv4 or IPv6 address
	}IPVersion;

	class NetworkInterfaceImpl;
	class Tuple
	{
	public:
		Tuple(const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress);
		~Tuple();
	public:
		const IPAddress& GetIPAddress(AddressType AddrType) const;
	protected:
		IPAddress	m_Addr;
		IPAddress	m_SubnetMask;
		IPAddress	m_BroadcastAddr;
	};

	class NET_API NetworkInterface
	{
	public:
		typedef std::vector<NetworkInterface>                List;
		typedef List																NetworkInterfaceList;//@deprecated
		typedef std::map<UINT, NetworkInterface>			Map;
		typedef Tuple															AddressTuple;
		typedef std::vector<AddressTuple>						AddressList;
		typedef AddressList::iterator									AddressIterator;
		typedef AddressList::const_iterator						ConstAddressIterator;
		typedef std::vector<BYTE>										MACAddress;

		static const unsigned NO_INDEX = ~0;
#if defined(POCO_OS_FAMILY_WINDOWS)
		static const char MAC_SEPARATOR = '-';
#else
		static const char MAC_SEPARATOR = ':';
#endif

		NetworkInterface(UINT uIndex = NO_INDEX);

		NetworkInterface(const NetworkInterface& InterFace);
		
		~NetworkInterface();

		NetworkInterface& operator = (const NetworkInterface& interfc);
		/// Assigns another NetworkInterface.

		BOOL operator < (const NetworkInterface& other) const;
		/// Operator less-than.

		BOOL operator == (const NetworkInterface& other) const;

		/// Swaps the NetworkInterface with another one.	
		void Swap(NetworkInterface& Other);

		/// Returns the interface OS index.
		UINT GetIndex() const;

		/// Returns the interface name.
		CString GetInterfaceName() const;

		/// Returns the interface display name.
		///
		/// On Windows platforms, this is currently the network adapter
		/// name. This may change to the "friendly name" of the network
		/// connection in a future version, however. 
		///
		/// On other platforms this is the same as name().
		CString GetInterfaceDisplayName() const;

		/// Returns the interface adapter name.
		///
		/// On Windows platforms, this is the network adapter LUID.
		/// The adapter name is used by some Windows Net APIs like DHCP. 
		///
		/// On other platforms this is the same as name().
		CString GetAdapterName() const;

		const IPAddress& GetFirstAddress(Family family) const;
		/// Returns the first IP address bound to the interface.
		/// Throws NotFoundException if the address family is not
		/// configured on the interface.

		const IPAddress& GetAddress(UINT uIndex = 0) const;
		/// Returns the IP address bound to the interface at index position.

		void GetAddress(const IPAddress& address);
		/// Adds address to the interface.

		void AddAddress(const IPAddress& Addr);

		void AddAddress(const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress);
		/// Adds address to the interface.

		const AddressList& GetAddressList() const;
		/// Returns the list of IP addresses bound to the interface.

		const IPAddress& GetSubnetMask(UINT uIndex = 0) const;
		/// Returns the subnet mask for this network interface.

		const IPAddress& GetBroadcastAddress(UINT uIndex = 0) const;
		/// Returns the broadcast address for this network interface.

		const IPAddress& GetDestAddress(UINT uIndex = 0) const;
		/// Returns the IPv4 point-to-point destination address for this network interface.

		const MACAddress& GetMacAddress() const;
		/// Returns MAC (Media Access Control) address for the interface.

		UINT GetMTU() const;
		/// Returns the MTU for this interface.

		NI_Type GetNIType() const;
		/// returns the MIB IfType of the interface.

		BOOL IsSupportsIP() const;
		/// Returns true if the interface supports IP.

		BOOL IsSupportsIPv4() const;
		/// Returns true if the interface supports IPv4.

		BOOL IsSupportsIPv6() const;
		/// Returns true if the interface supports IPv6.	

		BOOL IsSupportsBroadcast() const;
		/// Returns true if the interface supports broadcast.

		BOOL IsSupportsMulticast() const;
		/// Returns true if the interface supports multicast.

		BOOL IsLoopback() const;
		/// Returns true if the interface is loopback.

		BOOL IsPointToPoint() const;
		/// Returns true if the interface is point-to-point.

		BOOL IsRunning() const;
		/// Returns true if the interface is running.

		BOOL IsUp() const;
		/// Returns true if the interface is up.

		static NetworkInterface GetInterfaceByName(LPCTSTR lpszInterfaceName, BOOL bRequireIPv6 = FALSE);
		/// Returns the NetworkInterface for the given name.
		/// 
		/// If requireIPv6 is false, an IPv4 interface is returned.
		/// Otherwise, an IPv6 interface is returned.
		///
		/// Throws an InterfaceNotFoundException if an interface
		/// with the give name does not exist.

		static NetworkInterface GetInterfaceByName(LPCTSTR lpszInterfaceName, IPVersion ipVersion);
		/// Returns the NetworkInterface for the given name.
		/// 
		/// The ipVersion argument can be used to specify whether
		/// an IPv4 (IPv4_ONLY) or IPv6 (IPv6_ONLY) interface is required, 
		/// or whether the caller does not care (IPv4_OR_IPv6).
		///
		/// Throws an InterfaceNotFoundException if an interface
		/// with the give name does not exist.

		static NetworkInterface GetInterfaceByAddress(const IPAddress& Addr);
		/// Returns the NetworkInterface for the given IP address.
		///
		/// Throws an InterfaceNotFoundException if an interface
		/// with the give address does not exist.

		static NetworkInterface GetInterfaceByIndex(UINT uIndex);
		/// Returns the NetworkInterface for the given interface index.
		///
		/// Throws an InterfaceNotFoundException if an interface
		/// with the given index does not exist.

		static List GetList(BOOL bIPOnly = TRUE, BOOL bUPOnly = TRUE);
		/// Returns a list with all network interfaces
		/// on the system.
		///
		/// If ipOnly is true, only interfaces supporting IP
		/// are returned. Otherwise, all system network interfaces
		/// are returned.
		///
		/// If upOnly is true, only interfaces being up are returned.
		/// Otherwise, both interfaces being up and down are returned.
		///
		/// If there are multiple addresses bound to one interface,
		/// multiple NetworkInterface entries are listed for
		/// the same interface.

		static Map GetMap(BOOL bIPOnly = TRUE, BOOL bUPOnly = TRUE);
		/// Returns a map containing system network interfaces
		/// Map is keyed by interface system indices.
		///
		/// If ipOnly is true, only interfaces supporting IP
		/// are returned. Otherwise, all system network interfaces
		/// are returned.
		///
		/// If upOnly is true, only interfaces being up are returned.
		/// Otherwise, both interfaces being up and down are returned.
		///
		/// If there are multiple addresses bound to one interface,
		/// they are contained within the NetworkInterface (second) 
		/// member of the pair.

		protected:
			NetworkInterface(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, UINT uIndex, MACAddress* pMACAddress = 0);
			/// Creates the NetworkInterface.

			NetworkInterface(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, UINT uIndex, MACAddress* pMACAddress = 0);
			/// Creates the NetworkInterface.

			NetworkInterface(LPCTSTR lpszInterfaceName, const IPAddress& Addr, UINT uIndex, MACAddress* pMACAddress = 0);
			/// Creates the NetworkInterface.

			static IPAddress GetSubnetMaskByInterface(LPCTSTR lpszName, BOOL bIsLookback);

			NetworkInterface(LPCTSTR lpszInterfaceName,
										LPCTSTR lpszDisplayName,
										LPCTSTR lpszAdapterName,
										const IPAddress& Addr,
										const IPAddress& SubnetMask,
										const IPAddress& BroadcastAddress,
										UINT uIndex,
										MACAddress* pMACAddress = 0);
			/// Creates the NetworkInterface.

			NetworkInterface(LPCTSTR lpszInterfaceName,
				const IPAddress& Addr,
				const IPAddress& SubnetMask,
				const IPAddress& BroadcastAddress,
				UINT uIndex,
				MACAddress* pMACAddress = 0);
			/// Creates the NetworkInterface.

			IPAddress InterfaceNameToAddress(LPCTSTR lpszInterfaceName) const;
			/// Determines the IPAddress bound to the interface with the given name.

			UINT InterfaceNameToIndex(LPCTSTR lpszInterfaceName) const;
			/// Determines the interface index of the interface with the given name.

			NetworkInterfaceImpl& GetImpl() { return *_pImpl; };

	private:
		NetworkInterfaceImpl* _pImpl;

		//static Poco::FastMutex _mutex;
		};
}