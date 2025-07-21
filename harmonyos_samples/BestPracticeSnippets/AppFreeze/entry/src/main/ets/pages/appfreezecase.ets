/**
 * 最佳实践：应用冻屏优化建议
 */

// [Start appfreeze_advise2]
private getForeachKey(item: xxx) : string {
    // ...
    return `${item.xxx2}${item.xxx2}...${item.themeStyle}`;
} // 这部分逻辑如果较为耗时，执行次数多，总时长就是发生冻屏的耗时操作
// [End appfreeze_advise2]

/**
 * 最佳实践：应用冻屏优化建议
 */

// [Start appfreeze_advise3]
public static xxxFunction(fileUris:string[]): void {
    // ...
    for (const fileuril of fileUrils) {
        let file = fs.openSync(fileUri, fs.OpenMode.READ_ONLY);
        // ...
    }
    // ...
} // 如果使用同步操作，需要考虑到容器弱网或无网等极端情况发生
// [End appfreeze_advise3]