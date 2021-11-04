package com.xl.mmapclient

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.widget.TextView
import com.xl.mmapclient.databinding.ActivityMainBinding
import kotlin.concurrent.thread
import android.widget.Toast

import android.text.TextUtils


class MainActivity : AppCompatActivity() {
    companion object {
        // Used to load the 'mmapclient' library on application startup.
        init {
            System.loadLibrary("mmapclient")
        }
    }

    var isObser = false
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.btnOpen.setOnClickListener {
            mmapOpen("data/data/com.xl.mmapclient/mmaptest.txt")
        }
        binding.btnClose.setOnClickListener {
            mmapClose()
            isObser = false
        }
        binding.btnWrite.setOnClickListener {
            val content: String = binding.edit.text.toString()
            mmapWrite(content)
        }

        binding.btnObser.setOnClickListener {
            if (isObser) return@setOnClickListener
            isObser = true
            thread {
                while (isObser) {
                    try {
                        val observe = observe("")
                        if (!TextUtils.isEmpty(observe)) {
                            runOnUiThread {
                                binding.tvMsg.text = observe
                            }
                            //获取完之后将共享区内容置空
                            mmapSetEmpty()
                        }
                        Thread.sleep(500)
                    } catch (e: Exception) {

                    }
                }
            }
        }

    }

    /**
     * 客户端
     */
    private external fun stringFromJNI(): String

    external fun mmapOpen(path: String)

    external fun mmapClose()

    external fun mmapWrite(content: String)


    /**
     * 服务端
     */

    /**
     * 将映射区置空
     */
    external fun mmapSetEmpty()

    /**
     * 监听映射区的内容
     * @param defaultVal 传入的默认值
     * @return
     */
    external fun observe(defaultVal: String?): String?

}