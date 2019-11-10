# 安装node.js之后

## 临时使用
```
npm --registry https://registry.npm.taobao.org install express
```

## 持久使用
```
npm config set registry https://registry.npm.taobao.org

// 配置后可通过下面方式来验证是否成功
npm config get registry
// 或
npm info express
```

## 通过cnpm
## 通过cnpm
```
npm install -g cnpm --registry=https://registry.npm.taobao.org

// 使用
cnpm install expresstall express
```