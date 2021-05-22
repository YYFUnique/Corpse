#include "StdAfx.h"
#include "NetworkInterface.h"
#include <Ipifcons.h>
#include <IPTypes.h>

#pragma comment(lib,"Iphlpapi.lib")
namespace NetCore
{
	typedef NetworkInterface::AddressTuple	AddressTuple;
	typedef NetworkInterface::AddressList		InterfaceAddressList;
	typedef NetworkInterface::MACAddress	MACAddress;

	IPAddress GetBroadcastAddressEx(PIP_ADAPTER_PREFIX pPrefix, const IPAddress& addr, ULONG* pprefix /*= 0*/)
	{
		PIP_ADAPTER_PREFIX pPrev = 0;
		for (int i = 0; pPrefix; pPrefix = pPrefix->Next, ++i)
		{
			ADDRESS_FAMILY family = pPrefix->Address.lpSockaddr->sa_family;
			if ((family == AF_INET) && (addr == IPAddress(pPrefix->Address)))
				break;
			pPrev = pPrefix;
		}

		IPAddress Addr(IPv4);

		if (pPrefix && pPrefix->Next && pPrev)
		{
			IPAddress ipPrefix(pPrev->PrefixLength, IPv4);
			IPAddress mask(pPrefix->Next->Address);
			if ((ipPrefix & mask) == (ipPrefix & addr))
			{
				if (pprefix) *pprefix = pPrefix->PrefixLength;
				Addr = IPAddress(pPrefix->Next->Address);
			}
		}

		return Addr;
	}

	NI_Type NativeNiType(DWORD dwNiType)
	{
		switch (dwNiType)
		{
			case IF_TYPE_ETHERNET_CSMACD:    
				return NI_TYPE_ETHERNET_CSMACD;
			case IF_TYPE_ISO88025_TOKENRING: 
				return NI_TYPE_ISO88025_TOKENRING;
			case IF_TYPE_FRAMERELAY:         
				return NI_TYPE_FRAMERELAY;
			case IF_TYPE_PPP:                
				return NI_TYPE_PPP;
			case IF_TYPE_SOFTWARE_LOOPBACK:  
				return NI_TYPE_SOFTWARE_LOOPBACK;
			case IF_TYPE_ATM:               
				return NI_TYPE_ATM;
			case IF_TYPE_IEEE80211:         
				return NI_TYPE_IEEE80211;
			case IF_TYPE_TUNNEL:             
				return NI_TYPE_TUNNEL;
			case IF_TYPE_IEEE1394:           
				return NI_TYPE_IEEE1394;
			default:                         
				return NI_TYPE_OTHER;
		}
	}

	class NetworkInterfaceImpl : public RefCountObject
	{
	public:
		/*typedef NetworkInterface::Type					Type;*/

		NetworkInterfaceImpl(UINT uIndex);
		NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, UINT uIndex, MACAddress* pMACAddress = NULL);
		NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, UINT uIndex, MACAddress* pMACAddress = NULL);
		//NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress, UINT uIndex, MACAddress* pMACAddress = NULL);
		NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, 
											LPCTSTR lpszDisplayName,
											LPCTSTR lpszAdapterName,
											const IPAddress& Addr,
											const IPAddress& SubnetMask,
											const IPAddress& BroadcastAddress,
											UINT uIndex,
											MACAddress* pMACAddress = NULL);

		//UINT GetIndex() const;
		CString GetInterfaceName() const;
		CString GetInterfaceDisplayName() const;
		CString GetInterfaceAdapterName() const;
		const IPAddress& GetFirstAddress(Family family) const;
		void AddAddress(const AddressTuple& address);
		const IPAddress& GetAddress(UINT uIndex = 0) const;
		const InterfaceAddressList& GetAddressList() const;
		BOOL HasAddress(const IPAddress& address) const;
		const IPAddress& GetSubnetMask(UINT uIndex) const;
		const IPAddress& GetBroadcastAddress(UINT uIndex) const;
		const IPAddress& GetDestAddress(UINT uIndex) const;
		const MACAddress& GetMacAddress() const;
		BOOL IsSupportsIPv4() const;
		BOOL IsSupportsIPv6() const;

		void SetInterfaceName(LPCTSTR lpszInterfaceName);
		void SetInterfaceDisplayName(LPCTSTR lpszInterfaceDisplayName);
		void SetInterfaceAdapterName(LPCTSTR lpszInterfaceAdapterName);
		void AddAddress(const IPAddress& Addr);
		void SetInterfaceMACAddress(const MACAddress& addr);
		void SetInterfaceMACAddress(const void *pAddr, std::size_t len);

		UINT GetMTU() const;
		UINT GetInterfaceIndex() const;
		NI_Type GetNIType() const;

		BOOL IsBroadcast() const;
		BOOL IsLoopback() const;
		BOOL IsMulticast() const;
		BOOL IsPointToPoint() const;
		BOOL IsRunning() const;
		BOOL IsUP() const;

		void SetFlags(DWORD dwFlags, DWORD iftype);
		void SetRunning(BOOL IsRunning);

		void SetBroadcast(BOOL bBroadcast);
		void SetLoopback(BOOL bLoopback);
		void SetMulticast(BOOL bMulticast);
		void SetPointToPoint(BOOL bPointToPoint);
		void SetUp(BOOL bUP);
		void SetMTU(UINT uMTU);
		void SetNIType(NI_Type NIType);
		void SetIndex(UINT uIndex);
		void SetPhyParams();

	protected:
		~NetworkInterfaceImpl();

	private:
		CString		m_strInterfaceName;
		CString		m_strInterfaceDisplayName;
		CString		m_strInterfaceAdapterName;
		UINT		m_nInterfaceIndex;
		BOOL       m_bBroadcast;
		BOOL       m_bLoopback;
		BOOL       m_bMulticast;
		BOOL       m_bPointToPoint;
		BOOL		m_bUP;
		BOOL      m_IsRunning;
		UINT		m_nMTU;
		NI_Type   m_NIType;

		InterfaceAddressList	m_InterfaceAddressList;
		MACAddress				m_MacAddress;
	};

	Tuple::Tuple(const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress)
		: m_Addr(Addr)
		, m_SubnetMask(SubnetMask)
		, m_BroadcastAddr(BroadcastAddress)
	{

	}

	Tuple::~Tuple()
	{

	}

	const IPAddress& Tuple::GetIPAddress(AddressType AddrType) const
	{
		switch(AddrType)
		{
			case IP_ADDRESS:
					return m_Addr;
				break;
			case SUBNET_MASK:
					return m_SubnetMask;
				break;
			case BROADCAST_ADDRESS:
					return m_BroadcastAddr;
				break;
			default:
				return m_Addr;
		}		
	}

	NetworkInterfaceImpl::NetworkInterfaceImpl(UINT uIndex)
		: m_nInterfaceIndex(uIndex)
		, m_bBroadcast(FALSE)
		, m_bLoopback(FALSE)
		, m_bMulticast(FALSE)
		, m_bPointToPoint(FALSE)
		, m_bUP(FALSE)
		, m_IsRunning(FALSE)
		, m_nMTU(0)
		, m_NIType(NI_TYPE_OTHER)
	{

	}

	NetworkInterfaceImpl::NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, UINT uIndex, MACAddress* pMACAddress /* = NULL */)
		: m_strInterfaceName(lpszInterfaceName)
		, m_strInterfaceDisplayName(lpszDisplayName)
		, m_strInterfaceAdapterName(lpszAdapterName)
		, m_nInterfaceIndex(uIndex)
		, m_bBroadcast(FALSE)
		, m_bLoopback(FALSE)
		, m_bMulticast(FALSE)
		, m_bPointToPoint(FALSE)
		, m_bUP(FALSE)
		, m_IsRunning(FALSE)
		, m_nMTU(0)
		, m_NIType(NI_TYPE_OTHER)
	{
		m_InterfaceAddressList.push_back(AddressTuple(Addr, IPAddress(), IPAddress()));
		SetPhyParams();
		if (pMACAddress) SetInterfaceMACAddress(*pMACAddress);
	}

	NetworkInterfaceImpl::NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, UINT uIndex, MACAddress* pMACAddress /* = NULL */)
		: m_strInterfaceName(lpszInterfaceName)
		, m_strInterfaceDisplayName(lpszDisplayName)
		, m_strInterfaceAdapterName(lpszAdapterName)
		, m_nInterfaceIndex(uIndex)
		, m_bBroadcast(FALSE)
		, m_bLoopback(FALSE)
		, m_bMulticast(FALSE)
		, m_bPointToPoint(FALSE)
		, m_bUP(FALSE)
		, m_IsRunning(FALSE)
		, m_nMTU(0)
		, m_NIType(NI_TYPE_OTHER)
	{
		SetPhyParams();
		if (pMACAddress)
			SetInterfaceMACAddress(*pMACAddress);		
	}

	NetworkInterfaceImpl::NetworkInterfaceImpl(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress, UINT uIndex, MACAddress* pMACAddress /* = NULL */)
		: m_strInterfaceName(lpszInterfaceName)
		, m_strInterfaceDisplayName(lpszDisplayName)
		, m_strInterfaceAdapterName(lpszAdapterName)
		, m_nInterfaceIndex(uIndex)
		, m_bBroadcast(FALSE)
		, m_bLoopback(FALSE)
		, m_bMulticast(FALSE)
		, m_bPointToPoint(FALSE)
		, m_bUP(FALSE)
		, m_IsRunning(FALSE)
		, m_nMTU(0)
		, m_NIType(NI_TYPE_OTHER)
	{
		m_InterfaceAddressList.push_back(AddressTuple(Addr, SubnetMask, BroadcastAddress));
		SetPhyParams();
		if (pMACAddress) SetInterfaceMACAddress(*pMACAddress);
	}

	void NetworkInterfaceImpl::SetPhyParams()
	{
#if !defined(POCO_OS_FAMILY_WINDOWS) && !defined(POCO_VXWORKS)
		/*struct ifreq ifr;
		std::strncpy(ifr.ifr_name, _name.c_str(), IFNAMSIZ);
		DatagramSocket ds(SocketAddress::IPv4);

		ds.GetImpl()->ioctl(SIOCGIFFLAGS, &ifr);
		SetFlags(ifr.ifr_flags);

		ds.GetImpl()->ioctl(SIOCGIFMTU, &ifr);
		SetMTU(ifr.ifr_mtu);*/
#endif
	}

	NetworkInterfaceImpl::~NetworkInterfaceImpl()
	{

	}

	BOOL NetworkInterfaceImpl::IsSupportsIPv4() const
	{
		InterfaceAddressList::const_iterator it = m_InterfaceAddressList.begin();
		InterfaceAddressList::const_iterator end = m_InterfaceAddressList.end();
		for (; it != end; ++it)
		{
			const IPAddress& Addr = it->GetIPAddress(IP_ADDRESS);
			if (IPv4 == Addr.GetFamily())
				return TRUE;
		}

		return FALSE;
	}

	BOOL NetworkInterfaceImpl::IsSupportsIPv6() const
	{
#ifdef POCO_HAVE_IPv6
		AddressList::const_iterator it = _addressList.begin();
		AddressList::const_iterator end = _addressList.end();
		for (; it != end; ++it)
		{
			if (IPAddress::IPv6 == it->get<NetworkInterface::IP_ADDRESS>().family())
				return true;
		}
#endif
		return false;
	}

	UINT NetworkInterfaceImpl::GetInterfaceIndex() const
	{
		return m_nInterfaceIndex;
	}

	CString NetworkInterfaceImpl::GetInterfaceName() const
	{
		return m_strInterfaceName;
	}

	CString NetworkInterfaceImpl::GetInterfaceDisplayName() const
	{
		return m_strInterfaceDisplayName;
	}

	CString NetworkInterfaceImpl::GetInterfaceAdapterName() const
	{
		return m_strInterfaceAdapterName;
	}

	const IPAddress& NetworkInterfaceImpl::GetFirstAddress(Family family) const
	{
		InterfaceAddressList::const_iterator it = m_InterfaceAddressList.begin();
		InterfaceAddressList::const_iterator end = m_InterfaceAddressList.end();
		for (;it != end; ++it)
		{
			const IPAddress& Addr = it->GetIPAddress(IP_ADDRESS);
			if (Addr.GetFamily() == family) 
				return Addr;
		}
		return end->GetIPAddress(IP_ADDRESS);
	}

	void NetworkInterfaceImpl::AddAddress(const AddressTuple& address)
	{
		m_InterfaceAddressList.push_back(address);
	}

	BOOL NetworkInterfaceImpl::HasAddress(const IPAddress& Addr) const
	{
		InterfaceAddressList::const_iterator it = m_InterfaceAddressList.begin();
		InterfaceAddressList::const_iterator end = m_InterfaceAddressList.end();
		for (;it != end; ++it)
		{
			if (it->GetIPAddress(IP_ADDRESS) == Addr)
				return TRUE;
		}

		return FALSE;
	}

	const IPAddress& NetworkInterfaceImpl::GetAddress(UINT uIndex /*= 0*/) const
	{
		const IPAddress& Addr = m_InterfaceAddressList[0].GetIPAddress(IP_ADDRESS);
		if (uIndex < m_InterfaceAddressList.size()) 
			return m_InterfaceAddressList[uIndex].GetIPAddress(IP_ADDRESS);
		return Addr;
	}

	const IPAddress& NetworkInterfaceImpl::GetSubnetMask(UINT uIndex ) const
	{
		const IPAddress& Addr = m_InterfaceAddressList[0].GetIPAddress(SUBNET_MASK);
		if (uIndex < m_InterfaceAddressList.size()) 
			return m_InterfaceAddressList[uIndex].GetIPAddress(SUBNET_MASK);
		return Addr;
	}

	const IPAddress& NetworkInterfaceImpl::GetBroadcastAddress(UINT uIndex) const
	{
		const IPAddress& Addr = m_InterfaceAddressList[0].GetIPAddress(BROADCAST_ADDRESS);
		if (uIndex < m_InterfaceAddressList.size()) 
			return m_InterfaceAddressList[uIndex].GetIPAddress(BROADCAST_ADDRESS);
		return Addr;
	}

	const IPAddress& NetworkInterfaceImpl::GetDestAddress(UINT uIndex) const
	{
		const IPAddress& Addr = m_InterfaceAddressList[0].GetIPAddress(BROADCAST_ADDRESS);
		if (IsPointToPoint() == FALSE)
			return m_InterfaceAddressList[uIndex].GetIPAddress(BROADCAST_ADDRESS);

		return Addr;
	}

	const MACAddress& NetworkInterfaceImpl::GetMacAddress() const
	{
		return m_MacAddress;
	}

	const InterfaceAddressList& NetworkInterfaceImpl::GetAddressList() const
	{
		return m_InterfaceAddressList;
	}

	UINT NetworkInterfaceImpl::GetMTU() const
	{
		return m_nMTU;
	}

	NI_Type NetworkInterfaceImpl::GetNIType() const
	{
		return m_NIType;
	}

	BOOL NetworkInterfaceImpl::IsBroadcast() const
	{
		return m_bBroadcast;
	}

	BOOL NetworkInterfaceImpl::IsLoopback() const
	{
		return m_bLoopback;
	}

	BOOL NetworkInterfaceImpl::IsMulticast() const
	{
		return m_bMulticast;
	}

	BOOL NetworkInterfaceImpl::IsPointToPoint() const
	{
		return m_bPointToPoint;
	}

	BOOL NetworkInterfaceImpl::IsRunning() const
	{
		return m_IsRunning;
	}

	BOOL NetworkInterfaceImpl::IsUP() const
	{
		return m_bUP;
	}

	void NetworkInterfaceImpl::SetFlags(DWORD dwFlags, DWORD iftype)
	{
		m_IsRunning = m_bUP = FALSE;
		switch (iftype) 
		{
			case IF_TYPE_ETHERNET_CSMACD:
			case IF_TYPE_ISO88025_TOKENRING:
			case IF_TYPE_IEEE80211:
					m_bMulticast = m_bBroadcast = TRUE;
				break;
			case IF_TYPE_SOFTWARE_LOOPBACK:
					m_bLoopback =  TRUE;
				break;
			case IF_TYPE_PPP:
			case IF_TYPE_ATM:
			case IF_TYPE_TUNNEL:
			case IF_TYPE_IEEE1394:
					m_bPointToPoint = TRUE;
				break;
		}
		if (!(dwFlags & IP_ADAPTER_NO_MULTICAST))
			m_bMulticast = TRUE;
	}

	void NetworkInterfaceImpl::SetRunning(BOOL bRunning)
	{
		m_IsRunning = bRunning;
	}

	void NetworkInterfaceImpl::SetBroadcast(BOOL bBroadcast)
	{
		m_bBroadcast = bBroadcast;
	}

	void NetworkInterfaceImpl::SetLoopback(BOOL bLoopback)
	{
		m_bLoopback = bLoopback;
	}

	void NetworkInterfaceImpl::SetMulticast(BOOL bMulticast)
	{
		m_bMulticast = bMulticast;
	}

	void NetworkInterfaceImpl::SetPointToPoint(BOOL bPointToPoint)
	{
		m_bPointToPoint = bPointToPoint;
	}

	void NetworkInterfaceImpl::SetUp(BOOL bUP)
	{
		m_bUP = bUP;
	}

	void NetworkInterfaceImpl::SetMTU(UINT uMTU)
	{
		m_nMTU = uMTU;
	}

	void NetworkInterfaceImpl::SetNIType(NI_Type NiType)
	{
		m_NIType = NiType;
	}

	void NetworkInterfaceImpl::SetIndex(UINT uIndex)
	{
		m_nInterfaceIndex = uIndex;
	}

	void NetworkInterfaceImpl::SetInterfaceName(LPCTSTR lpszInterfaceName)
	{
		m_strInterfaceName = lpszInterfaceName;
	}

	void NetworkInterfaceImpl::SetInterfaceDisplayName(LPCTSTR lpszInterfaceDisplayName)
	{
		m_strInterfaceDisplayName = lpszInterfaceDisplayName;
	}

	void NetworkInterfaceImpl::SetInterfaceAdapterName(LPCTSTR lpszInterfaceAdapterName)
	{
		m_strInterfaceAdapterName = lpszInterfaceAdapterName;
	}

	void NetworkInterfaceImpl::AddAddress(const IPAddress &Addr)
	{
		m_InterfaceAddressList.push_back(AddressTuple(Addr, IPAddress(), IPAddress()));
	}

	void NetworkInterfaceImpl::SetInterfaceMACAddress(const MACAddress& addr)
	{
		m_MacAddress = addr;
	}

	void NetworkInterfaceImpl::SetInterfaceMACAddress(const void *pAddr, std::size_t len)
	{
		const unsigned char* pMacAddr =  static_cast<const unsigned char*>(pAddr);
		m_MacAddress.clear();
		m_MacAddress.insert(m_MacAddress.end(), pMacAddr, pMacAddr + len);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//// NetworkInterface
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL NetworkInterface::operator < (const NetworkInterface& other) const
	{
		return this->GetIndex() < other.GetIndex();
	}

	BOOL NetworkInterface::operator == (const NetworkInterface& other) const
	{
		return this->GetIndex() == other.GetIndex();
	}

	NetworkInterface::NetworkInterface(UINT uIndex)
		: _pImpl(new NetworkInterfaceImpl(uIndex))
	{

	}

	NetworkInterface::NetworkInterface(const NetworkInterface& InterFace)
		: _pImpl(InterFace._pImpl)
	{
		_pImpl->Duplicate();
	}

	NetworkInterface::NetworkInterface(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, UINT uIndex, MACAddress* pMACAddress /* = 0 */)
		: _pImpl(new NetworkInterfaceImpl(lpszInterfaceName, lpszDisplayName, lpszAdapterName, uIndex, pMACAddress))
	{

	}

	NetworkInterface::NetworkInterface(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, UINT uIndex, MACAddress* pMACAddress /* = 0 */)
		: _pImpl(new NetworkInterfaceImpl(lpszInterfaceName, lpszInterfaceName, lpszInterfaceName, Addr, uIndex, pMACAddress))
	{

	}

	NetworkInterface::NetworkInterface(LPCTSTR lpszInterfaceName, const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress, UINT uIndex, MACAddress* pMACAddress /* = 0 */)
		: _pImpl(new NetworkInterfaceImpl(lpszInterfaceName, lpszInterfaceName, lpszInterfaceName, Addr, SubnetMask, BroadcastAddress, uIndex, pMACAddress))
	{

	}

	NetworkInterface::NetworkInterface(LPCTSTR lpszInterfaceName, LPCTSTR lpszDisplayName, LPCTSTR lpszAdapterName, const IPAddress& Addr, const IPAddress& SubnetMask, const IPAddress& BroadcastAddress, UINT uIndex, MACAddress* pMACAddress /* = 0 */)
		: _pImpl(new NetworkInterfaceImpl(lpszInterfaceName, lpszDisplayName, lpszAdapterName, Addr, SubnetMask, BroadcastAddress, uIndex, pMACAddress))
	{

	}

	NetworkInterface::~NetworkInterface()
	{
		_pImpl->Release();
	}

	NetworkInterface& NetworkInterface::operator = (const NetworkInterface&  interfc)
	{
		NetworkInterface tmp(interfc);
		Swap(tmp);
		return *this;
	}

	void NetworkInterface::Swap(NetworkInterface& Other)
	{
		std::swap(_pImpl, Other._pImpl);
	}

	UINT NetworkInterface::GetIndex() const
	{
		return _pImpl->GetInterfaceIndex();
	}

	CString NetworkInterface::GetInterfaceName() const
	{
		return _pImpl->GetInterfaceName();
	}

	CString NetworkInterface::GetInterfaceDisplayName() const
	{
		return _pImpl->GetInterfaceDisplayName();
	}

	CString NetworkInterface::GetAdapterName() const
	{
		return _pImpl->GetInterfaceAdapterName();
	}

	const IPAddress& NetworkInterface::GetFirstAddress(Family family) const
	{
		return _pImpl->GetFirstAddress(family);
	}

	/*void NetworkInterface::GetFirstAddress()*/

	void NetworkInterface::AddAddress(const IPAddress& Addr)
	{
		_pImpl->AddAddress(AddressTuple(Addr, IPAddress(), IPAddress()));
	}

	void NetworkInterface::AddAddress(const IPAddress &Addr, const IPAddress &SubnetMask, const IPAddress &BroadcastAddress)
	{
		_pImpl->AddAddress(AddressTuple(Addr, SubnetMask, BroadcastAddress));
	}

	const IPAddress& NetworkInterface::GetAddress(UINT uIndex/* = 0*/) const
	{
		return _pImpl->GetAddress(uIndex);
	}
	
	const NetworkInterface::AddressList& NetworkInterface::GetAddressList() const
	{
		return _pImpl->GetAddressList();
	}

	const IPAddress& NetworkInterface::GetSubnetMask(UINT uIndex /* = 0 */) const
	{
		return _pImpl->GetSubnetMask(uIndex);
	}

	const IPAddress& NetworkInterface::GetBroadcastAddress(UINT uIndex /* = 0 */) const
	{
		return _pImpl->GetBroadcastAddress(uIndex);
	}

	const NetworkInterface::MACAddress& NetworkInterface::GetMacAddress() const
	{
		return _pImpl->GetMacAddress();
	}

	const IPAddress& NetworkInterface::GetDestAddress(UINT uIndex /* = 0 */) const
	{
		return _pImpl->GetDestAddress(uIndex);
	}

	UINT NetworkInterface::GetMTU() const
	{
		return _pImpl->GetMTU();
	}

	NI_Type NetworkInterface::GetNIType() const
	{
		return _pImpl->GetNIType();
	}

	BOOL NetworkInterface::IsSupportsIP() const
	{
		return IsSupportsIPv4() || IsSupportsIPv6();
	}

	BOOL NetworkInterface::IsSupportsIPv4() const
	{
		return _pImpl->IsSupportsIPv4();
	}

	BOOL NetworkInterface::IsSupportsIPv6() const
	{
		return _pImpl->IsSupportsIPv6();
	}

	BOOL NetworkInterface::IsSupportsMulticast() const
	{
		return _pImpl->IsMulticast();
	}

	BOOL NetworkInterface::IsLoopback() const
	{
		return _pImpl->IsLoopback();
	}

	BOOL NetworkInterface::IsPointToPoint() const
	{
		return _pImpl->IsPointToPoint();
	}

	BOOL NetworkInterface::IsRunning() const
	{
		return _pImpl->IsRunning();
	}

	BOOL NetworkInterface::IsUp() const
	{
		return _pImpl->IsUP();
	}

	NetworkInterface NetworkInterface::GetInterfaceByName(LPCTSTR lpszInterfaceName, BOOL bRequireIPv6 /* = FALSE */)
	{
		if (bRequireIPv6)
			return GetInterfaceByName(lpszInterfaceName, IPv6_ONLY);
		else
			return GetInterfaceByName(lpszInterfaceName, IPv4_OR_IPv6);
	}

	NetworkInterface NetworkInterface::GetInterfaceByName(LPCTSTR lpszInterfaceName, IPVersion ipVersion)
	{
		Map map = NetworkInterface::GetMap(false, false);
		Map::const_iterator it = map.begin();
		Map::const_iterator end = map.end();

		for (; it != end; ++it)
		{
			if (_tcsicmp(it->second.GetInterfaceName(), lpszInterfaceName) == 0)
			{
				if (ipVersion == IPv4_ONLY && it->second.IsSupportsIPv4())
					return it->second;
				else if (ipVersion == IPv6_ONLY && it->second.IsSupportsIPv6())
					return it->second;
				else if (ipVersion == IPv4_OR_IPv6)
					return it->second;
			}
		}
		
		// ·µ»Ø¿Õ
		return end->second;
	}

	NetworkInterface NetworkInterface::GetInterfaceByIndex(UINT uIndex)
	{
		Map map = NetworkInterface::GetMap(false, false);
		Map::const_iterator it = map.find(uIndex);
		if (uIndex == NetworkInterface::NO_INDEX)
			return map.end()->second;

		if (it != map.end())
			return it->second;
		else
			return map.end()->second;
	}

	NetworkInterface NetworkInterface::GetInterfaceByAddress(const IPAddress& Addr)
	{
		Map map = NetworkInterface::GetMap(TRUE, FALSE);
		Map::const_iterator it = map.begin();
		Map::const_iterator end = map.end();

		NetworkInterface interfc = end->second;

		for (; it != end; ++it)
		{
			const std::size_t count = it->second.GetAddressList().size();
			for (UINT n = 0; n < count; ++n)
			{
				if (it->second.GetAddress(n) == Addr)
					return it->second;
			}
		}

		return interfc;
	}

	NetworkInterface::List NetworkInterface::GetList(BOOL bIPOnly /* = TRUE */, BOOL bUPOnly /* = TRUE */)
	{
		List Interfacelist;
		Map m = GetMap(bIPOnly, bUPOnly);
		NetworkInterface::Map::const_iterator it = m.begin();
		NetworkInterface::Map::const_iterator end = m.end();

		for (; it != end; ++it)
		{
			int nIndex = it->second.GetIndex();
			CString strInterfaceName = it->second.GetInterfaceName();
			CString strDisplayName = it->second.GetInterfaceDisplayName();
			CString strAdapterName = it->second.GetAdapterName();

			MACAddress Mac = it->second.GetMacAddress();

			const AddressList& ipList = it->second.GetAddressList();
			if (ipList.size() > 0)
			{
				NetworkInterface::AddressList::const_iterator ipIt = ipList.begin();
				NetworkInterface::AddressList::const_iterator ipEnd = ipList.end();

				for (; ipIt != ipEnd; ++ipIt)
				{
					IPAddress Addr = ipIt->GetIPAddress(IP_ADDRESS);
					IPAddress Mask = ipIt->GetIPAddress(SUBNET_MASK);

					NetworkInterface ni;
					if (Mask.IsWildcard())
						ni = NetworkInterface(strInterfaceName, strDisplayName, strAdapterName, Addr, nIndex, &Mac);
					else
					{
						IPAddress BroadCast = ipIt->GetIPAddress(BROADCAST_ADDRESS);
						ni = NetworkInterface(strInterfaceName, strDisplayName, strAdapterName, Addr, Mask, BroadCast, nIndex, &Mac);
					}

					ni._pImpl->SetBroadcast(it->second._pImpl->IsBroadcast());
					ni._pImpl->SetLoopback(it->second._pImpl->IsLoopback());
					ni._pImpl->SetMulticast(it->second._pImpl->IsMulticast());
					ni._pImpl->SetPointToPoint(it->second._pImpl->IsPointToPoint());
					ni._pImpl->SetUp(it->second._pImpl->IsUP());
					ni._pImpl->SetRunning(it->second._pImpl->IsRunning());
					ni._pImpl->SetMTU(it->second._pImpl->GetMTU());
					ni._pImpl->SetNIType(it->second._pImpl->GetNIType());

					Interfacelist.push_back(ni);
				}
			}
			else
			{
				Interfacelist.push_back(NetworkInterface(strInterfaceName, strDisplayName, strAdapterName, nIndex, &Mac));
			}
		}

		return Interfacelist;
	}

	IPAddress NetworkInterface::GetSubnetMaskByInterface(LPCTSTR lpszName, BOOL bIsLookback)
	{
		if (bIsLookback)
			return IPAddress::Parse(_T("255.0.0.0"));

		return IPAddress::Parse(_T("255.255.255.0"));
	}

	NetworkInterface::Map NetworkInterface::GetMap(BOOL bIPOnly /*= TRUE*/, BOOL bUPOnly /*= TRUE*/)
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);

		//TODO: ¼ÓËø£¿
		Map ResultMap;
		ULONG uBufLen = 0;
		ULONG uFlags = (GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_PREFIX);
#ifdef GAA_FLAG_INCLUDE_ALL_INTERFACES
		uFlags |= GAA_FLAG_INCLUDE_ALL_INTERFACES;
#endif

#ifdef POCO_HAVE_IPv6
		const unsigned family = AF_UNSPEC;	// IPv4 and IPv6
#else
		const unsigned family = AF_INET;			// IPv4 only
#endif

		DWORD dwRetVal = 0;
		ULONG iterations = 0;
		PIP_ADAPTER_ADDRESSES pAddress = NULL;

		do 
		{
			dwRetVal = GetAdaptersAddresses(family, uFlags, 0, pAddress, &uBufLen);
			if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
				if (pAddress != NULL)
				{
					delete[] pAddress;
					pAddress = NULL;
				}
				pAddress = (PIP_ADAPTER_ADDRESSES)new BYTE[uBufLen];
				if (pAddress == NULL)
					break;
				dwRetVal = GetAdaptersAddresses(family, uFlags, 0, pAddress, &uBufLen);
			} 
		} while ((ERROR_BUFFER_OVERFLOW == dwRetVal) && (++iterations <= 2));

		for (; pAddress; pAddress = pAddress->Next)
		{
			IPAddress Addr;
			IPAddress SubnetMask;
			IPAddress BroadcastAddr;
			UINT ifIndex = 0;
#ifdef POCO_HAVE_IPv6
			if ((osvi.dwMajorVersion >= 5) &&
				(osvi.dwMinorVersion >= 1) && 
				(osvi.dwBuildNumber >= 1))
				ifIndex = pAddress->Ipv6IfIndex;
#endif

			if (osvi.dwMajorVersion >= 6)
			{
				if (pAddress->Flags & IP_ADAPTER_IPV4_ENABLED)
					ifIndex = pAddress->IfIndex;
			}
			else
				ifIndex = pAddress->IfIndex;

			if (ifIndex == 0)
				continue;

			CString strAdapterName(pAddress->AdapterName);
			CString strFriendlyName(pAddress->FriendlyName);
			CString strDisplayName(pAddress->Description);

			BOOL bIsUP = pAddress->OperStatus == IfOperStatusUp;
			BOOL bIsIP = (pAddress->FirstUnicastAddress != 0);
			if (((bIPOnly && bIsIP) || bIPOnly == FALSE) && ((bUPOnly && bIsUP) || bUPOnly == FALSE))
			{
				NetworkInterface ni(strFriendlyName, strDisplayName, strAdapterName, ifIndex);
				Map::iterator ifIt = ResultMap.find(ifIndex);
				if (ifIt == ResultMap.end())
					ifIt = ResultMap.insert(Map::value_type(ifIndex, ni)).first;

				ifIt->second.GetImpl().SetFlags(pAddress->Flags, pAddress->IfType);
				ifIt->second.GetImpl().SetMTU(pAddress->Mtu);
				ifIt->second.GetImpl().SetUp(pAddress->OperStatus == IfOperStatusUp);
#if (_WIN32_WINNT >= 0x0600)	//Vista and newer only
				if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 0 && osvi.dwBuildNumber >= 0)
					ifIt->second.GetImpl().SetRunning(pAddress->ReceiveLinkSpeed > 0 || pAddress->TransmitLinkSpeed >0);
#endif
				ifIt->second.GetImpl().SetNIType(NativeNiType(pAddress->IfType));
				if (pAddress->PhysicalAddressLength)
					ifIt->second.GetImpl().SetInterfaceMACAddress(pAddress->PhysicalAddress, pAddress->PhysicalAddressLength);

				for (PIP_ADAPTER_UNICAST_ADDRESS pUniAddr = pAddress->FirstUnicastAddress;
					pUniAddr; pUniAddr = pUniAddr->Next)
				{
					Addr = IPAddress(pUniAddr->Address);
					ADDRESS_FAMILY family = pUniAddr->Address.lpSockaddr->sa_family;
					switch(family)
					{
						case AF_INET:
						{
							BOOL bHasBroadcast = (pAddress->IfType == IF_TYPE_ETHERNET_CSMACD) || 
																	(pAddress->IfType == IF_TYPE_SOFTWARE_LOOPBACK) ||
																	(pAddress->IfType == IF_TYPE_IEEE80211);
							if (bHasBroadcast)
							{
								ULONG uPrefixLen = 0;

#if (_WIN32_WINNT >= 0x0600)	//Vista and newer
								if (osvi.dwMajorVersion >= 6)
								{
									uPrefixLen = pUniAddr->OnLinkPrefixLength;
									BroadcastAddr = GetBroadcastAddressEx(pAddress->FirstPrefix, Addr);
								}
								else
									BroadcastAddr = GetBroadcastAddressEx(pAddress->FirstPrefix, Addr, &uPrefixLen);
#else
								BroadcastAddr = GetBroadcastAddressEx(pAddress->FirstPrefix, Addr, &uPrefixLen);
#endif
								ASSERT(uPrefixLen <= 32);
								if (BroadcastAddr.IsWildcard())
								{
									IPAddress Mask(uPrefixLen, IPv4);
									IPAddress Host(Mask & Addr);
									BroadcastAddr = Host | ~Mask;
								}

								if (uPrefixLen)
									SubnetMask = IPAddress(uPrefixLen, IPv4);
								else
								{
									Addr = IPAddress(&reinterpret_cast<SOCKADDR_IN*>(pUniAddr->Address.lpSockaddr)->sin_addr, sizeof(IN_ADDR));
									SubnetMask = GetSubnetMaskByInterface(strFriendlyName, Addr.IsLoopback());
									if (Addr.IsLoopback() == FALSE)
									{
										BroadcastAddr = Addr;
										BroadcastAddr.Mask(SubnetMask, IPAddress::GetBroadcast());
									}
								}
								ifIt->second.AddAddress(Addr, SubnetMask, BroadcastAddr);
							}
							else
								ifIt->second.AddAddress(Addr);
						}
#if defined(POCO_HAVE_IPv6)
						case AF_INET6:
							ifIt->second.AddAddress(Addr);
							break;
#endif
					}
				}
			}
		}

		return ResultMap;
	}
}