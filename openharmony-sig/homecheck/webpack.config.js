/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const TsconfigPathsPlugin = require('tsconfig-paths-webpack-plugin');
const TerserPlugin = require('terser-webpack-plugin');
const nodeExternals = require('webpack-node-externals');
const path = require('path');
const webpack = require('webpack');

module.exports = (env, argv) => {
    let outputPath = path.resolve(__dirname, 'dist');
    let devtoolModuleFilenameTemplate = '../[resource-path]';
    const builder = env.builder ? env.builder : 'Community';

    const config = {
        name: 'HomeCheck',
        target: 'node',
        mode: 'argv.mode',
        entry: { index: './src/index.ts' },
        module: {
            rules: [{
                test: /\.ts$/,
                exclude: /node_modules/,
                loader: 'ts-loader',
                options: { onlyCompileBundledFiles: true, }
            }],
            exprContextCritical: false,
        },
        resolve: {
            extensions: ['.ts', '.js', '.json'],
            plugins: [new TsconfigPathsPlugin()],
        },
        output: {
            path: outputPath,
            filename: '[name].js',
            libraryTarget: 'commonjs2',
            devtoolModuleFilenameTemplate,
        },
        plugins: [
            new webpack.DefinePlugin({ 
                MODE_PRODUCTION: JSON.stringify(argv.mode === 'production'),
                'process.env': {
                    BUILDER: JSON.stringify(builder),
                }
            }),
            new CleanWebpackPlugin({ cleanStaleWebpackAssets: false, }),
        ],
        externals: [
        ],
    };

    if (argv.mode === 'production') {
        config.plugins.push(
            new TerserPlugin({
                terserOptions: { sourceMap: true },
                extractComments: false,
            }),
            new webpack.DefinePlugin({ MODE_PRODUCTION: JSON.stringify(true), })
        );
    }
    return config;
};