import java.net.URL;
import java.net.HttpURLConnection;
import java.io.*;
import java.nio.charset.Charset;
import java.util.stream.Collectors;

public class Demo {
    public static void requestCommon(String method) throws Exception {
        String query = "http://127.0.0.1:3100";
        String json = "{\"key\":1}";

        URL url = new URL(query);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setConnectTimeout(5000);
        conn.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
        conn.setDoOutput(true);
        conn.setDoInput(true);
        conn.setRequestMethod(method);

        OutputStream os = conn.getOutputStream();
        os.write(json.getBytes("UTF-8"));
        os.close();

        // read the response
        Charset charset = Charset.defaultCharset();
        InputStream in = new BufferedInputStream(conn.getInputStream());
        String resp = "";
        try (BufferedReader br = new BufferedReader(new InputStreamReader(in, charset))) {
           resp = br.lines().collect(Collectors.joining(System.lineSeparator()));
        }

        System.out.println(resp);

        in.close();
        conn.disconnect();
    }

    public static void main(String[] args) throws Exception {
        requestCommon("GET");
        requestCommon("POST");
        requestCommon("DELETE");
    }
}
