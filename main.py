import logging
import discord
import subprocess
from lib import process_output
from discord.ext import commands


class msg(commands.Cog):

    def __init__(self, bot):
        self.bot = bot
        self.logger = logging.getLogger(__name__)

    @commands.command(aliases=["bb"])
    async def buildbasic(self, ctx):
        msg = "BUILD FROM BASIC LANG IN C++\n"
        m: discord.Message = await ctx.send(msg)
        p = subprocess.Popen(["make","-C","./plugin/BASIC_lang/"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        _ = await process_output(p, m, msg, ctx)
        await ctx.send("ENDED.")
    
    @commands.command(aliases=["basic"])
    async def runbasic(self, ctx, *args):
        msg = ''
        program = ''
        
        if args[0] != '```':
            await ctx.send('syntax error')
        
        for i in args[1:]:
            if len(i) == 0 or i == '```':
                break
            
            program += i + '\n'
        
        await ctx.send(str(program))
    
    @commands.command(aliases=["db"])
    async def debugbasic(self, ctx):
        msg = "DEBUG FROM BASIC LANG IN C++\n"
        m: discord.Message = await ctx.send(msg)
        p = subprocess.Popen(["make","-C","./plugin/BASIC_lang/"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        _ = await process_output(p, m, msg, ctx)
        await ctx.send("ENDED.")
        msg = "chmod a+x ./basic\n"
        m: discord.Message = await ctx.send(msg)
        p = subprocess.Popen(["chmod","a+x","./plugin/BASIC_lang/basic"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        _ = await process_output(p, m, msg, ctx)
        await ctx.send("ENDED.")
        msg = "RUN DEBUG\n"
        m: discord.Message = await ctx.send(msg)
        p = subprocess.Popen(["./plugin/BASIC_lang/basic"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        _ = await process_output(p, m, msg, ctx)
        _ = p.communicate()[0]
        await ctx.send("ENDED. CODE:"+str(p.returncode))


def setup(bot):
    bot.add_cog(msg(bot))
