package br.xadrez.academico;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

/** Abre o xadrez.html (e o Stockfish WASM) de dentro do APK, servindo os assets como um site seguro. */
public class MainActivity extends Activity {
    private static final String HOST = "appassets.androidplatform.net";
    private WebView web;

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        web = new WebView(this);
        web.setBackgroundColor(0xFFE8EDF2);
        setContentView(web);
        WebSettings s = web.getSettings();
        s.setJavaScriptEnabled(true);
        s.setDomStorageEnabled(true);   // guarda o aprendizado da IA
        s.setAllowFileAccess(false);
        web.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView v, WebResourceRequest r) {
                if (!HOST.equals(r.getUrl().getHost())) return null;
                String path = r.getUrl().getPath();
                if (path == null || !path.startsWith("/assets/")) return null;
                String name = path.substring(8);
                Map<String, String> h = new HashMap<>();
                // cabeçalhos que liberam o Stockfish multi-thread (SharedArrayBuffer)
                h.put("Cross-Origin-Opener-Policy", "same-origin");
                h.put("Cross-Origin-Embedder-Policy", "require-corp");
                h.put("Cross-Origin-Resource-Policy", "same-origin");
                try {
                    InputStream in = getAssets().open(name);
                    return new WebResourceResponse(mime(name), name.endsWith(".wasm") ? null : "utf-8", 200, "OK", h, in);
                } catch (IOException e) {
                    return new WebResourceResponse("text/plain", "utf-8", 404, "Not Found", h, new ByteArrayInputStream(new byte[0]));
                }
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView v, WebResourceRequest r) {
                return !HOST.equals(r.getUrl().getHost());
            }
        });
        web.loadUrl("https://" + HOST + "/assets/xadrez.html");
    }

    private static String mime(String n) {
        if (n.endsWith(".html")) return "text/html";
        if (n.endsWith(".js")) return "text/javascript";
        if (n.endsWith(".wasm")) return "application/wasm";
        if (n.endsWith(".png")) return "image/png";
        return "application/octet-stream";
    }

    @Override
    public void onBackPressed() {
        // volta ao menu do app; no menu, fecha
        web.evaluateJavascript("(typeof view!=='undefined'&&view!=='menu')?(go('menu'),'1'):'0'", v -> {
            if (!"\"1\"".equals(v)) finish();
        });
    }
}
