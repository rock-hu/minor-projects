/**
 * Bridge权限配置
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 10:48
 */
export interface BridgePermission {
  /**
   * 桥名称
   */
  name: string
  /**
   * 桥中有权限的方法，其中allAllow表示有所有方法权限
   */
  methods: string
}