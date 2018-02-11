package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"path/filepath"
	"strings"
	"time"

	"github.com/if1live/maya"
	"github.com/op/go-logging"
)

func main() {
	startTime := time.Now()

	logging.SetLevel(logging.CRITICAL, "maya")

	srcs, err := findArticles("content")
	if err != nil {
		panic(err)
	}

	for _, src := range srcs {
		pathinfo := NewPathInfo(src)
		cmd := NewArticleCommand(pathinfo)
		cmd.execute()
	}

	endTime := time.Now()
	elapsedTime := endTime.Sub(startTime)
	fmt.Println("elapsed time:", elapsedTime.String())
}

// https://gist.github.com/francoishill/a5aca2a7bd598ef5b563
func findArticles(searchDir string) ([]string, error) {
	fileList := []string{}
	err := filepath.Walk(searchDir, func(path string, f os.FileInfo, err error) error {
		fileList = append(fileList, path)
		return nil
	})

	articleFiles := []string{}
	for _, file := range fileList {
		if strings.HasSuffix(file, ".mkdn") {
			articleFiles = append(articleFiles, file)
		}
	}

	return articleFiles, err
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
		"cache",
		"Makefile",
		"requirements.txt",
		"Gemfile",
		"Gemfile.lock",
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
	err := os.MkdirAll(dstDir, 0644)
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
	err = os.MkdirAll(c.pathinfo.outputDir(), 0644)
	if err != nil {
		panic(err)
	}

	// write article
	output := modifyMarkdown(article.OutputString())
	data := []byte(output)

	outfile, err := os.Create(c.pathinfo.outputFilePath())
	if err != nil {
		panic(err)
	}
	defer outfile.Close()
	writer := bufio.NewWriter(outfile)
	writer.Write(data)
	writer.Flush()
}

func modifyMarkdown(text string) string {
	text = strings.Replace(text, "{attach}", "/posts/", -1)
	text = strings.Replace(text, "{filename}", "/posts/", -1)
	text = strings.Replace(text, "{article}", "/posts/", -1)
	text = strings.Replace(text, ".md", "", -1)
	return text
}
