#pragma once

namespace NetCore
{
	typedef enum tagSignatureMethod
	{
		SIGN_PLAINTEXT,		/// OAuth 1.0A PLAINTEXT signature method
		SIGN_HMAC_SHA1,	/// OAuth 1.0A HMAC-SHA1 signature method
	}SignatureMethod;

	class NET_API OAuth10Credentials
	{
	public:
		OAuth10Credentials();
		OAuth10Credentials(LPCTSTR lpszConsumerKey, LPCTSTR lpszConsumerSecret);
		OAuth10Credentials(LPCTSTR lpszConsumerKey, LPCTSTR lpszConsumerSecret, LPCTSTR lpszToken, LPCTSTR lpszTokenSecret);
		explicit OAuth10Credentials(const HTTPRequest& Request);
		~OAuth10Credentials();

	public:
		void SetConsumerKey(LPCTSTR lpszConsumerKey);
		LPCTSTR GetConsumerKey() const;

		void SetConsumerSecret(LPCTSTR lpszConsumerSecret);
		LPCTSTR GetConsumerSecret() const;

		void SetToken(LPCTSTR lpszToken);
		LPCTSTR GetToken() const;

		void SetTokenSecret(LPCTSTR lpszTokenSecret);
		LPCTSTR GetTokenSecret() const;

		void SetRealm(LPCTSTR lpszRealm);
		LPCTSTR GetRealm() const;

		void SetCallback(LPCTSTR lpszCallbackURI);
		LPCTSTR GetCallback() const;

		void Authenticate();
		void Authenticate();

		//BOOL Verify(const HTTPRequest& Request, )

	protected:
		//void SignPlainText(HTTPRequest& Request) const;
		//void SignHMACSHA1(HTTPRequest& Request)
	};
}