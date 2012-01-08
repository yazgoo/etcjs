#!/usr/bin/env ruby
require 'socket'
require 'tmpdir'
require 'time'
require 'test/unit'
require 'fileutils'
class RestALittle < TCPSocket
    def initialize port
        5.times do
            begin
                super 'localhost', port
                break
            rescue Errno::ECONNREFUSED
                sleep 0.05
            end
        end
    end
    def post action, post
        puts "POST #{action} HTTP/1.1"
        puts "Content-Length: #{post.size}"
        puts ""
        puts post
        code = gets
        code = code.split[1].to_i
        while (line = gets.chomp) and line != ""
        end 
        data = ""
        while (size = gets.chomp.hex.to_i) != 0
            data += read(size).chomp
        end
        yield code, data
    end
end
class TestServer < Test::Unit::TestCase
    def setup
        @store = File.join Dir.tmpdir, "store#{Time.new.to_i}"
        @port = 1337
        Dir.mkdir @store
        @io = IO.popen "node ../src/server/etc.js #{@port} #{@store}"
    end
    def teardown
        Process.kill 'TERM', @io.pid
        @io.close
        FileUtils.rm_rf @store
    end
    def post action, req
        cl = RestALittle.new @port
        cl.post(action, req) { |code, data| yield code, data }
        cl.close
    end
    def test_all
        post "/config/get", "owner=ownie&name=conf&key=foo" do |code, data|
            assert_equal 401, code
            assert_not_nil data
        end
        post "/owner/create", "name=ownie&key=foo" do |code, data|
            assert_equal 200, code
            assert_not_nil data
        end
        post "/config/set", "owner=ownie&key=wrong&name=conf&content=lol" do |code, data|
            assert_equal 401, code
            assert_not_nil data
        end
        post "/config/set", "owner=ownie&key=foo&name=conf&content=lol" do |code, data|
            assert_equal 200, code
            assert_not_nil data
        end
        post "/config/get", "owner=ownie&key=foo&name=conf" do |code, data|
            assert_equal 200, code
            assert_equal "lol", data
        end
        post "/config/list", "owner=ownie&key=foo" do |code, data|
            assert_equal 200, code
            assert_equal "conf", data
        end
    end
end
