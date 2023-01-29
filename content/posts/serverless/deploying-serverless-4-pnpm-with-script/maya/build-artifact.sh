#!/bin/bash

set -exuo pipefail

DIR_TMP=artifact

function prepare {
	mkdir -p $DIR_TMP
	cp package.json pnpm-*.yaml $DIR_TMP
	cp -r .webpack/src $DIR_TMP
}

function install_package {
	pushd $DIR_TMP

	pnpm install --prod --frozen-lockfile --offline

	# 불필요한 패키지, 파일 삭제
	rm -rf node_modules/.pnpm/@types+*
	rm -rf node_modules/.pnpm/figlet@*
	rm -rf node_modules/.pnpm/aws-sdk@*

	# https://unix.stackexchange.com/a/622875
	find node_modules/.pnpm -depth -name "test" -type d -execdir rm -rf {} +
	find node_modules/.pnpm -depth -name "tests" -type d -execdir rm -rf {} +
	find node_modules/.pnpm -depth -name "__tests__" -type d -execdir rm -rf {} +

	find node_modules/.pnpm -name "*.ts" -type f -delete
	find node_modules/.pnpm -name "*.map" -type f -delete
	find node_modules/.pnpm -name "*test*" -type f -delete

	popd
}

function archive_artifact {
	pushd $DIR_TMP
	zip -r --symlinks -q ../artifact.zip node_modules src
	popd
}

prepare
install_package
archive_artifact
