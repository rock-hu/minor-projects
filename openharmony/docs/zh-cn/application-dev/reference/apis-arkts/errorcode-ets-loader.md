# ���빤����������

> **˵����**
>
> ���½����ܱ�ģ�����д����룬ͨ�ô�������ο�[ͨ�ô�����˵���ĵ�](../errorcode-universal.md)��

## 10311001 ����ArkTS�ļ�����

**������Ϣ**

Importing ArkTS files in JS and TS files is forbidden.

**��������**

��ֹ��JS��TS�ļ��е���ArkTS�ļ���

**����ԭ��**

��JS��TS�ļ��е�����ArkTS�ļ���

**������**

���Ƴ�JS��TS�ļ��е�ArkTS�ļ�������䡣

## 10311002 ����OhmUrl����

**������Ϣ**

Failed to resolve OhmUrl. Failed to get a resolved OhmUrl for `${filePath}` imported by `${importerFile}`.

**��������**

�޷�����OhmUrl���޷�Ϊ`${importerFile}`�����`${filePath}`��ȡ�������OhmUrl��

**����ԭ��**

`${filePath}`������`${pkgName}`ģ�����ô���

**������**

1. ���`${filePath}`������`${pkgName}`ģ���Ƿ�������ȷ��
2. ����Ӧ���ļ����Ƿ���ȷ��������Сд���У���

## 10311003 ��ȡKit�����ļ�����

**������Ϣ**

Kit `${moduleRequest}` has no corresponding config file in ArkTS SDK.

**��������**

Kit `${moduleRequest}` ��ArkTS SDK��û�ж�Ӧ�������ļ���

**����ԭ��**

1. Kit��API��SDK��һ�¡�
2. Kit��API�ڱ��ر��޸ġ�

**������**

1. ��ȷ��Kit��API��SDKһ�£�����Kit��APIû�б������޸ġ�
2. �й�Kit API�ĸ�����ϸ��Ϣ����ο�[����˵��](../development-intro.md)��

## 10311005 ����Kit��ʶ������

**������Ϣ**

Identifier `${this.importName}` comes from `${this.symbol.source}` which can not be imported in .ts file.

**��������**

��ʶ��`${this.importName}`����`${this.symbol.source}`���ñ�ʶ��������.ts�ļ��е��롣

**����ԭ��**

��TS�ļ��е�����ArkTS�ļ���Kitģ�顣

**������**

���Ƴ����������ļ���չ������Ϊ.ets��

## 10311006 ��������δ��Kit�е�������

**������Ϣ**

`${importName}` is not exported from Kit `${KitInfo.getCurrentKitName()}`.

**��������**

`${importName}`û�д�Kit `${KitInfo.getCurrentKitName()}` �е�����

**����ԭ��**

1. Kit��API��SDK��һ�¡�
2. Kit��API�ڱ��ر��޸ġ�

**������**

1. ��ȷ��Kit��API��SDKһ�£�����Kit��APIû�б������޸ġ�
2. �й�Kit API�ĸ�����ϸ��Ϣ����ο�[����˵��](../development-intro.md)��

## 10311007 Kit�����ռ䵼��򵼳�����

**������Ϣ**

Namespace import or export of Kit is not supported currently.

**��������**

��ǰ��֧��Kit�������ռ䵼��򵼳���

**����ԭ��**

�ļ���ʹ����Kit�������ռ䵼��򵼳���

**������**

�뽫Kit�������ռ䵼��򵼳��滻Ϊ��������򵼳������磬`import * as ArkTS from "@kit.ArkUI";` -> `import { AlertDialog } from "@kit.ArkUI";`��

## 10311008 Kit�յ������

**������Ϣ**

Can not use empty import(side-effect import) statement with Kit `${(kitNode.moduleSpecifier as ts.StringLiteral).text.replace(/'|"/g, '')}`.

**��������**

�޷���Kit `${(kitNode.moduleSpecifier as ts.StringLiteral).text.replace(/'|"/g, '')}` ��ʹ�ÿյ��루�����õ��룩��䡣

**����ԭ��**

�ļ���ʹ����Kit�Ŀյ��루�����õ��룩��䡣

**������**

����ȷָ������ķ��š����磬`import "@kit.ArkUI";` -> `import { lang } from "@kit.ArkUI";`��

## 10311009 ES2ABCִ�д���

**������Ϣ**

Failed to execute es2abc.

**��������**

�޷�ִ��es2abc��

**����ԭ��**

es2abcִ�й����г��ִ���

**������**

��ο�es2abc�Ĵ�������д���

## 10311010 �����ص������µ�������

**������Ϣ**

`${elementText}` of lazy-import is re-export.

**��������**

`${elementText}`�������ص��뱻���µ�����

**����ԭ��**

���µ����˾��������ص����`${elementText}`��

**������**

1. ��ȷ�������ص����������û�б����µ�����
2. �����Ƿ�����autoLazyImport���ء�
