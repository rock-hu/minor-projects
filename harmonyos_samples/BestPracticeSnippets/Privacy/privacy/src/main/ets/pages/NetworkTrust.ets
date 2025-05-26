import { BusinessError } from '@kit.BasicServicesKit'
import { http } from '@kit.NetworkKit'
import { rcp } from '@kit.RemoteCommunicationKit';

let httpRequest = http.createHttp();
/**
 * 最佳实践：网络连接安全配置开发实践
 * 场景四：配置信任应用管理的CA证书-Network Kit
 */
// [Start cybersecurity_network_trusting_ca]
httpRequest.request( "EXAMPLE_URL",  {
  method: http.RequestMethod.POST,
  header: {
    'Content-Type': 'application/json'
  },
  extraData: "data to send",
  expectDataType: http.HttpDataType.STRING,
  connectTimeout: 60000,
  caPath:'/res/domainCaCert', // Specifies the trusted CA certificate path
}, (err: BusinessError, data: http.HttpResponse) => {
  // ...
})
// [End cybersecurity_network_trusting_ca]

/**
 * 最佳实践：网络连接安全配置开发实践
 * 场景五：配置信任应用管理的CA证书-Remote Communication Kit
 */

// [Start cybersecurity_rcp_trusting_ca]
const caPath: rcp.CertificateAuthority = {
  folderPath: '/res/appCaCert', // 指定信任的CA证书路径
}
const securityConfig: rcp.SecurityConfiguration = {
  remoteValidation: caPath
};
// Use the security configuration in the session creation
const sessionWithSecurityConfig = rcp.createSession({ requestConfiguration: { security: securityConfig } });
// [End cybersecurity_rcp_trusting_ca]

let HTTP_SERVER = '1'
const RemoteCommunicationKit = async  () => {
  /**
   * 最佳实践：网络连接安全配置开发实践
   * 场景九：配置SSL Pinning证书锁定-Remote Communication Kit配置动态SSL Pinning
   */
  // [Start rcp_ssl_pinning]
  const keyHash: string = 'g8CsdcpyAKxmLoWFvMd2hC7ZDUy7L4E2NYOi1i8qEtE='; //服务器证书的公钥
  const session = rcp.createSession();
  const request = new rcp.Request(HTTP_SERVER);
  const pin: rcp.CertificatePinning = {
    kind: 'public-key',
    publicKeyHash: keyHash,
    hashAlgorithm: 'SHA-256'
  };
  request.configuration = {
    security: {
      certificatePinning: pin,
    }
  };
  const resp = await session.fetch(request);
  // [End rcp_ssl_pinning]
}

