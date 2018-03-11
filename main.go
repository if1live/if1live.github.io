package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"path/filepath"
	"regexp"
	"strings"
	"time"

	"github.com/if1live/maya"
	"github.com/op/go-logging"
)

func main() {
	startTime := time.Now()

	logging.SetLevel(logging.CRITICAL, "maya")
	buildMayaArticles()

	endTime := time.Now()
	elapsedTime := endTime.Sub(startTime)
	fmt.Println("elapsed time:", elapsedTime.String())
}

func mergeArticleAndMetadata() {
	srcs, err := findArticles("content", func(file string) bool {
		return strings.HasSuffix(file, "metadata.yml")
	})

	if err != nil {
		panic(err)
	}

	for _, src := range srcs {
		pathinfo := NewMetadataPathInfo(src)
		if !pathinfo.isUpdated() {
			continue
		}
		cmd := NewMergeCommand(pathinfo)
		cmd.execute()
	}
}
func deleteTempFiles() {
	srcs, err := findArticles("content", func(file string) bool {
		return strings.HasSuffix(file, "metadata.yml")
	})

	if err != nil {
		panic(err)
	}

	for _, src := range srcs {
		pathinfo := NewMetadataPathInfo(src)
		cmd := NewDeleteTempCommand(pathinfo)
		cmd.execute()
	}
}

func buildMayaArticles() {
	srcs, err := findArticles("content", func(file string) bool {
		return strings.HasSuffix(file, ".mkdn")
	})

	if err != nil {
		panic(err)
	}

	for _, src := range srcs {
		pathinfo := NewPathInfo(src)
		if !pathinfo.isUpdated() {
			continue
		}
		cmd := NewArticleCommand(pathinfo)
		cmd.execute()
	}
}

// https://gist.github.com/francoishill/a5aca2a7bd598ef5b563
func findArticles(searchDir string, fn func(string) bool) ([]string, error) {
	fileList := []string{}
	err := filepath.Walk(searchDir, func(path string, f os.FileInfo, err error) error {
		fileList = append(fileList, path)
		return nil
	})

	articleFiles := []string{}
	for _, file := range fileList {
		if fn(file) {
			articleFiles = append(articleFiles, file)
		}
	}

	return articleFiles, err
}

type MetadataPathInfo struct {
	metadataFilePath string
	sourceDir        string
	sourceFile       string
}

func NewMetadataPathInfo(fp string) *MetadataPathInfo {
	absfp, err := filepath.Abs(fp)
	if err != nil {
		panic(err)
	}
	absfp = strings.Replace(absfp, `\`, "/", -1)
	sourceDir, sourceFile := path.Split(absfp)

	return &MetadataPathInfo{
		metadataFilePath: absfp,
		sourceDir:        sourceDir,
		sourceFile:       sourceFile,
	}
}

func (pi *MetadataPathInfo) documentFilePath() string {
	return path.Join(pi.sourceDir, "document.mkdown")
}
func (pi *MetadataPathInfo) mayaFilePath() string {
	return path.Join(pi.sourceDir, "article.mkdn")
}

func (pi *MetadataPathInfo) isUpdated() bool {
	target, err := os.Stat(pi.mayaFilePath())
	if err != nil {
		return true
	}

	metadata, err := os.Stat(pi.metadataFilePath)
	if err != nil {
		panic(err)
	}
	if target.ModTime().UnixNano() < metadata.ModTime().UnixNano() {
		return true
	}

	document, err := os.Stat(pi.documentFilePath())
	if err != nil {
		panic(err)
	}
	if target.ModTime().UnixNano() < document.ModTime().UnixNano() {
		return true
	}

	return false
}

type PathInfo struct {
	sourceFilePath string
	sourceDir      string
	sourceFile     string

	slug     string
	category string

	rootPath   string
	targetFile string
}

// NewPathInfo is ...
// example
// content/black-magic/double-loop-with-array/article.mkdn
// category : black-magic
// slug : double-loop-with-array
// filename : article.mkdn
func NewPathInfo(fp string) *PathInfo {
	absfp, err := filepath.Abs(fp)
	if err != nil {
		panic(err)
	}
	absfp = strings.Replace(absfp, `\`, "/", -1)

	tokens := strings.Split(absfp, "/")
	sourceDir, sourceFile := path.Split(absfp)
	slug := tokens[len(tokens)-2]
	category := tokens[len(tokens)-3]

	rootPath := ""
	for len(tokens) > 0 {
		remain, last := tokens[:len(tokens)-1], tokens[len(tokens)-1]
		if last == "content" {
			rootPath = strings.Join(tokens[:len(tokens)-1], "/")
			break
		}
		tokens = remain
	}

	return &PathInfo{
		sourceFilePath: absfp,
		sourceDir:      sourceDir,
		sourceFile:     sourceFile,

		slug:     slug,
		category: category,

		rootPath:   rootPath,
		targetFile: fmt.Sprintf("%s.md", slug),
	}
}

func (pi *PathInfo) isUpdated() bool {
	target, err := os.Stat(pi.outputFilePath())
	if err != nil {
		fmt.Println(err)
		return true
	}
	source, err := os.Stat(pi.sourceFilePath)
	if err != nil {
		panic(err)
	}
	if target.ModTime().UnixNano() < source.ModTime().UnixNano() {
		return true
	}
	return false
}

// todo use abs path
func (pi *PathInfo) outputDir() string {
	return path.Join(pi.rootPath, "hugo", "content", pi.category)
}

func (pi *PathInfo) outputFilePath() string {
	dir := pi.outputDir()
	fp := path.Join(dir, pi.targetFile)
	return fp
}
func (pi *PathInfo) staticDir() string {
	return path.Join(pi.rootPath, "hugo", "static", "posts", pi.slug)
}
func (pi *PathInfo) staticFiles() []string {
	files, err := ioutil.ReadDir(pi.sourceDir)
	if err != nil {
		panic(err)
	}

	statics := []string{}
	for _, file := range files {
		if file.IsDir() {
			continue
		}
		if pi.isIgnorableStaticFileName(file.Name()) {
			continue
		}

		statics = append(statics, file.Name())
	}
	return statics
}

func (pi *PathInfo) isIgnorableStaticFileName(name string) bool {
	names := []string{
		"article.mkdn",
		"document.mkdown",
		"cache",
		"Makefile",
		"requirements.txt",
		"Gemfile",
		"Gemfile.lock",
		"metadata.yml",
	}
	prefixs := []string{}
	suffixs := []string{
		".cpp",
		".js",
		".c",
		".lisp",
		".php",
		".py",
		".rb",
		".cs",
		".il",
		".asm",
		".sh",
		".h",
		".txt",
		".go",
		".java",
		".log",
		".s",
	}

	for _, n := range names {
		if n == name {
			return true
		}
	}
	for _, prefix := range prefixs {
		if strings.HasPrefix(name, prefix) {
			return true
		}
	}
	for _, suffix := range suffixs {
		if strings.HasSuffix(name, suffix) {
			return true
		}
	}
	return false
}

type MergeCommand struct {
	pathinfo *MetadataPathInfo
}

func NewMergeCommand(pi *MetadataPathInfo) *MergeCommand {
	return &MergeCommand{pi}
}

func (c *MergeCommand) execute() {
	metadata, err := ioutil.ReadFile(c.pathinfo.metadataFilePath)
	if err != nil {
		panic(err)
	}

	document, err := ioutil.ReadFile(c.pathinfo.documentFilePath())
	if err != nil {
		panic(err)
	}

	var buffer bytes.Buffer
	buffer.WriteString("---\n")
	buffer.Write(metadata)
	buffer.WriteString("---\n")
	buffer.Write(document)

	ioutil.WriteFile(c.pathinfo.mayaFilePath(), buffer.Bytes(), 0644)
}

type DeleteTempCommand struct {
	pathinfo *MetadataPathInfo
}

func NewDeleteTempCommand(pathinfo *MetadataPathInfo) *DeleteTempCommand {
	return &DeleteTempCommand{pathinfo}
}

func (c *DeleteTempCommand) execute() {
	os.Remove(c.pathinfo.mayaFilePath())
}

type ArticleCommand struct {
	pathinfo *PathInfo
}

func NewArticleCommand(pathinfo *PathInfo) *ArticleCommand {
	return &ArticleCommand{
		pathinfo: pathinfo,
	}
}

func (c *ArticleCommand) execute() {
	fmt.Printf("execute start : %s\n", c.pathinfo.sourceFilePath)
	c.writeDocument()
	c.copyStatic()
	fmt.Printf("execute finish : %s\n", c.pathinfo.sourceFilePath)
}

func (c *ArticleCommand) copyStatic() {
	staticFiles := c.pathinfo.staticFiles()
	if len(staticFiles) == 0 {
		return
	}

	dstDir := c.pathinfo.staticDir()
	srcDir := c.pathinfo.sourceDir

	// prepare static directory
	err := os.MkdirAll(dstDir, 0755)
	if err != nil {
		panic(err)
	}

	// copy static files
	for _, file := range staticFiles {
		src := path.Join(srcDir, file)
		dst := path.Join(dstDir, file)

		data, err := ioutil.ReadFile(src)
		if err != nil {
			panic(err)
		}

		err = ioutil.WriteFile(dst, data, 0644)
		if err != nil {
			panic(err)
		}
	}
}

func (c *ArticleCommand) writeDocument() {
	// save working directory
	wd, _ := os.Getwd()
	defer os.Chdir(wd)

	err := os.Chdir(c.pathinfo.sourceDir)
	if err != nil {
		panic(err)
	}

	infile, err := os.Open(c.pathinfo.sourceFile)
	if err != nil {
		panic(err)
	}
	defer infile.Close()
	reader := bufio.NewReader(infile)
	article := maya.NewArticleFromReader(reader, maya.ModeHugo)

	// prepare output directory
	err = os.MkdirAll(c.pathinfo.outputDir(), 0755)
	if err != nil {
		panic(err)
	}

	// write article
	output := modifyMarkdown(article.OutputString())
	data := []byte(output)
	ioutil.WriteFile(c.pathinfo.outputFilePath(), data, 0644)
}

func modifyMarkdown(text string) string {
	text = strings.Replace(text, "{attach}", "/posts/", -1)
	text = strings.Replace(text, "{filename}", "/posts/", -1)
	text = strings.Replace(text, "{article}", "/posts/", -1)
	text = strings.Replace(text, ".md", "", -1)

	// post-processing
	// \\-> \\\{space}
	// to render LaTeX
	re := regexp.MustCompile(`\\\\\n`)
	text = re.ReplaceAllString(text, `\\\ `+"\n")

	return text
}
