package com.sta.unityrenderer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.sta.unityrenderer.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.sampleText.text = buildString {
            appendLine("UnityRenderer - Native Unity UI Framework")
            appendLine("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
            appendLine()
            appendLine("📋 Status: Library Loaded")
            appendLine("🏗️ Architecture: ${System.getProperty("os.arch")}")
            appendLine("📱 Android: ${android.os.Build.VERSION.RELEASE} (API ${android.os.Build.VERSION.SDK_INT})")
            appendLine("📦 Package: ${packageName}")
            appendLine()
            appendLine("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
            appendLine("This is an educational project demonstrating")
            appendLine("IL2CPP runtime interaction from native C++.")
            appendLine()
            appendLine("Features:")
            appendLine("• Hook-free UI creation via il2cpp_thread_attach")
            appendLine("• Modular widget system (Toggle, SeekBar, Checkbox)")
            appendLine("• Touch input with drag-to-move panel")
            appendLine("• OOP architecture with RAII memory management")
            appendLine()
            appendLine("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
            appendLine("🔗 github.com/springmusk026/UnityRenderer")
        }
    }

}
