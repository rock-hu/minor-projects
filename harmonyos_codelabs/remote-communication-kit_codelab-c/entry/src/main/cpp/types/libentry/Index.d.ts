export const Get: (url: string) => string;
export const GetWithTimeout: (url: string, connectTimeoutMs: number, transferTimeout: number) => string;
export const GetWithTransferRange: (url: string, from: number, to:number) => string;